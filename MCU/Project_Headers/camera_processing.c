/*
 * camera_processing.c
 *
 *  Created on: Aug 19, 2014
 *      Author: B48923
 */

#include "camera_processing.h"
#include "chrono.h"
#include "Serial/serial.h"
#include "TFC/TFC_ADC.h"
#include "TFC/TFC_ADC.h"

#define TFC_LINESCAN0_ADC_CHANNEL	6

void init_data(cameraData* data)
{
	int i;
	
	int32_t hardcoded_derivative_zero[128] = 
	{
		0, 74, 31, 67, 41, 38, 26, 45,
		4,  0, 26, 22,-10, 58,-10, 41,
	  -13, 48,-11, 18, 15, 19,-11, 27,
	   -7, 41,-23, 38,-13, 47,-32, 28,
	    4, 26,-22, 29,-30, 38,-12, 12,
	  -27, 40,-25, 22, -8, 17,-23, 38,
	  -41, 53,-33, 15, -3, 29,-25,  7,
	   -1,	9,-10, 10, -4, 16,-16, 16,
	  -17, 23,-17,  9,-29, 42,-35, 33,
	  -32, 27,-22, -3,  3, 12,-25, 17,
	  -13,  3, -8, 23,-28, 14, -5, 10,
	  -14,	0, -3, -7,-23, 16,-22,-10,
	   -6,-12,-10,  0,-18,-14,-17, -3,
	  -20, -4,-23,  3,-32,  0,-27,-15,
	  -17, -1,-28,-17,-21, -8,-45, 12,
	  -50,  0,-34,-15,-42,-15,-54,  0

		
	};
	
	//Init image buffer
	for(i=0;i<128;i++)
	{
		data->raw_image[i] = 0;
		data->derivative_image[i] = 0;
		data->threshold_image[i] = 0;
		data->falling_edges_position[i] = 0;
		data->rising_edges_position[i] = 0;
		data->derivative_zero[i] = hardcoded_derivative_zero[i];
	}
	
	data->edges_count = 0;
	data->line_position = 0;
	data->previous_line_position = 0;
	
	data->threshold = 100;
	data->halftrack_width = 110;
	data->offset = 0.f;
	data->linewidth = 14.f;
	data->linewidth_margin = 2;
	data->deglitch_counter = 0;
	data->deglitch_limit = 35;
	data->filter_coeff = 0.7;
	
	data->edgeleft = 1;//MIN VALUE : 1
	data->edgeright = 1;//MIN VALUE : 1	
}

void read_data(cameraData* data)
{
	uint8_t clock_cycles = 50;
	uint32_t i,j;
		
	
	//Start new measurement for next conversion
	//Signal sequence in the camera datasheet
		
	TAOS_SI_HIGH;
	//Wait for camera settling time 
	for(j = 0; j < clock_cycles ; j++)
	{
		
	}
	TAOS_CLK_HIGH;
	//Wait for camera settling time 
	for(j = 0; j < clock_cycles ; j++)
	{
		
	}
	TAOS_SI_LOW;
	ADC0_CFG2  |= ADC_CFG2_MUXSEL_MASK; //Select the B side of the mux
	ADC0_SC1A  =  TFC_LINESCAN0_ADC_CHANNEL;
	TAOS_CLK_LOW;
	
	//Start reading pixel one by one with the ADC
	for(i = 0 ; i < 128 ; i++)
	{		
		//Wait for end of ADC conversion... A TESTER
		while ((ADC_SC1_REG(ADC0_BASE_PTR,0) & ADC_SC1_COCO_MASK ) == 0x00)
		{
		}
		
		//Lecture du registre de conversion
		data->raw_image[i] = ADC0_RA;		
				
		//New pixel
		TAOS_CLK_HIGH;
		
		//Wait for camera settling time 
		for(j = 0; j < clock_cycles ; j++)
		{
			
		}
				
		//Start new conversion
		ADC0_CFG2  |= ADC_CFG2_MUXSEL_MASK;
		ADC0_SC1A  =  TFC_LINESCAN0_ADC_CHANNEL;
		
		TAOS_CLK_LOW;
	}
}

int process_data(cameraData* data)
{	
	float position;
	uint8_t edge_signal;
	int16_t i;
	int16_t loopright = 128 - data->edgeright;
	
	data->edges_count = 0;
	edge_signal = 0;
	data->derivative_image[data->edgeleft] = data->raw_image[data->edgeleft];
	data->threshold_image[data->edgeleft-1] = 0;
	
	for(i = data->edgeleft ; i < loopright ; i++)
	{
		data->derivative_image[i] =  (int32_t)(data->raw_image[i+1]) - (int32_t)(data->raw_image[i]) - data->derivative_zero[i];
		
		//Filtered value formula
		//data->filtered_raw[i] = data->filtered_raw[i-1] * data->filter_coeff + (float)(data->derivative_image[i]) * (1.f - data->filter_coeff);
		
		//Apply threshold
		if(data->derivative_image[i] >  data->threshold)
			data->threshold_image[i] = 2;
		//White to black
		else if(data->derivative_image[i] < -data->threshold)
			data->threshold_image[i] = 1;
		else
			data->threshold_image[i] = -1;
		
		//Look for falling then rising edge, from pixel 0 to 128
		//Edge detected
		if(data->threshold_image[i-1] * data->threshold_image[i] < 0)
		{
			//Too many edges detected, error
			if(data->edges_count > 6)
			{
				return LINE_LOST;
			}
			
			//First edge detected (edges go by pair)
			if(edge_signal == 0)
			{
				edge_signal = 1;
				//Record position
				//TODO : Renommer first edge second edge
				data->falling_edges_position[data->edges_count] = i;
			}
			else
			{
				edge_signal = 0;
				//Record position
				data->rising_edges_position[data->edges_count] = i;
				data->edges_count++;
			}
		}
	}
	
	//Process detected edges
	if(data->edges_count == 0)
	{
		//No edge detected, keep line position
		return LINE_LOST;
	}
	else if(data->edges_count == 1)
	{
		return LINE_LOST;
		/*
		//Line is in very border of camera, or car is going outside the road
		position = (float)(data->rising_edges_position[0] + data->falling_edges_position[0]) / 2.f - 64.f; 
		
		//Estimate new position for both cases (Car on the left or car on the right)
		//White to black
		if(data->threshold_image[data->falling_edges_position[0]] == 1)
		{
			//Si la position est > -64+linewidth  on a forcement le bord droit
			if(position > -64 + data->linewidth * 1.2f)
			{
				data->one_edge_choice = 2;
				data->line_position = position + data->offset - data->halftrack_width;
				return LINE_OK;
			}
			else
			{
				data->one_edge_choice = 0;
				return LINE_LOST;
			}
				
			
			//data->position_left = position;
			//data->position_right = position + data->offset - data->halftrack_width;//OK, sur la droite la valeur part bien en negatif
		}
		//Black to white
		else if(data->threshold_image[data->falling_edges_position[0]] == 2)
		{
			if(position < 64 - data->linewidth * 1.2f)
			{
				data->one_edge_choice = -2;
				data->line_position = position + data->offset + data->halftrack_width;
				return LINE_OK;
			}
			else
			{
				data->one_edge_choice = 0;
				return LINE_LOST;
			}
				
			
			//data->position_left = position + data->offset + data->halftrack_width;
			//data->position_right = position;
		}*/
	}
	else if(data->edges_count == 2)
	{
		data->deglitch_counter = 0;
		//Check derivative order 
		
		//Work with twice the linewidth and int32
		data->current_linewidth = (data->rising_edges_position[0] + data->falling_edges_position[0]) / 2  -
						  	  	  (data->rising_edges_position[1] + data->falling_edges_position[1]) / 2;
		
		if(data->current_linewidth < 0)
			data->current_linewidth *= -1;
		
		data->current_linewidth_diff = data->current_linewidth - data->linewidth;
		
		if(data->current_linewidth_diff > data->linewidth_margin || data->current_linewidth_diff < -data->linewidth_margin)
			return LINE_LOST;
		
		//1 edge - compute center & record
		position = (data->rising_edges_position[0] + data->falling_edges_position[0] + data->rising_edges_position[1] + data->falling_edges_position[1]) / 4.f;
		position -= 64;
		data->line_position = position + data->offset;
		
		return LINE_OK;
	}
	/*
	else if(data->edges_count == 6)
	{
		return LINE_LOST;
		//TODO : CHECK INTERVALS ARE CORRECT 
		position = (data->rising_edges_position[2] + data->falling_edges_position[2] + data->rising_edges_position[3] + data->falling_edges_position[3]) / 4.f;
		position -= 64;
		data->line_position = position + data->offset;
		//TODO : CALL FUNCTION TO SIGNAL START LINE
		return LINE_OK;
	}*/
	return LINE_LOST;
}

void calibrate_data(cameraData* data, uint32_t exposure_time_us)
{
	int counter = 0;
	chrono chr;
	chrono chr_exposure;
	int32_t i; 
	reset(&chr);
	int32_t dz[128];
	
	for(i = 0 ; i < 128 ; i++)
	{
		data->derivative_zero[i] = 0;
		dz[i] = 0;
	}
	
	//Wait 1 second
	while(ms(&chr) < 1000)
	{
		
	}
	
	reset(&chr);
	
	//Empty camera sensor
	process_data(data);
	reset(&chr_exposure);
	
	/*-------------------------------*/
	/*       DERIVATIVE ZERO         */
	/*-------------------------------*/
	while(counter < 200 && ms(&chr) < 5000)
	{
		if(us(&chr_exposure) > exposure_time_us)
		{
			reset(&chr_exposure);
			read_data(data);
			if(process_data(data) == LINE_LOST)
			{
				// Sum derivative
				if(data->edges_count == 0)
				{
					for(i = data->edgeleft ; i < 128 - data->edgeright ; i++)
					{
						dz[i] += data->derivative_image[i];
					}
					counter++;
				}
			}
			
		}
	}
	
	if(counter == 0)
	{
		serial_printf("Calibration step 2 : ISSUE : %d \n",counter);
	}
	else
	{
		serial_printf("Lens compensation done. Averaging %d times\n",counter);
	
		for(i = 0 ; i < 128 ; i++)
		{
		data->derivative_zero[i] = dz[i] / counter;
		}
	}
}


