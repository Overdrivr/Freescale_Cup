/*
 * camera_processing.c
 *
 *  Created on: Aug 19, 2014
 *      Author: B48923
 */

#include "camera_processing.h"
#include "chrono.h"
#include "Serial/serial.h"

void init_data(cameraData* data)
{
	int i;
	//Init image buffer
	for(i=0;i<128;i++)
	{
		data->raw_image[i] = 0;
		data->derivative_image[i] = 0;
		data->threshold_image[i] = 0;
		data->falling_edges_position[i] = 0;
		data->rising_edges_position[i] = 0;
		data->derivative_zero[i] = 0;
	}
	data->edges_count = 0;
	data->line_position = 0;
	data->valid_line_position = 0;
	data->previous_line_position = 0;
	data->image_integral = 0;
	data->error = 0;
	data->reference_integral = 0;//Compute reference integral at calibration
	
	data->threshold = 200;
	data->halftrack_width = 150;
	data->offset = 0.f;
	data->linewidth = 14.f;
	data->deglitch_counter = 0;
	data->deglitch_limit = 50;
	
	data->edgeleft = 1;//MIN VALUE : 1
	data->edgeright = 1;//MIN VALUE : 1
	data->hysteresis_threshold = 50;
	
}


int read_process_data(cameraData* data)
{	
	float position;
	uint8_t edge_signal;
	int16_t i;
	int16_t loopright = 128 - data->edgeright;
		
	data->image_integral = 0;
	
	// Copy image, compute integral & min/max
	for(i=0;i<128;i++)
	{
		data->raw_image[i] = LineScanImage0[i];
		
		data->image_integral += data->raw_image[i];
		
		if(data->raw_image[i] > data->max)
			data->max = data->raw_image[i];
		if(data->raw_image[i] < data->min)
			data->min =data->raw_image[i];
	}
	
	//If signal integral is too small, the line is very likely lost
	//if(data->image_integral < data->linewidth * data->threshold * 0.5)
	//	return LINE_LOST;
	
	data->edges_count = 0;
	edge_signal = 0;
	data->derivative_image[data->edgeleft] = data->raw_image[data->edgeleft];
	data->threshold_image[data->edgeleft-1] = 0;
	data->threshold_image[loopright] = 0;
	
	for(i = data->edgeleft ; i < loopright ; i++)
	{
		if(i < 127)
			data->derivative_image[i] =  (int32_t)(data->raw_image[i+1]) - (int32_t)(data->raw_image[i]) - data->derivative_zero[i];
		
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
		//Line is in very border of camera, or car is going outside the road
		position = (float)(data->rising_edges_position[0] + data->falling_edges_position[0]) / 2.f - 64.f;
		
		//Zone d'incertitude droite
		/*if(position > 0 && position + data->linewidth > 40)
		{
			if(data->linestate > 0)
				data->linestate = LINE_HALF_TRACK_RIGHT;
			else
				data->linestate = LINE_HALF_LEFT;
				
			return LINE_LOST;
		}*/
		//Zone d'incertitude gauche
		// Impossible de dire si on a affaire au bord de piste ou a la moitie de la ligne
		/*else if(position < 0 && position - data->linewidth < -40) 
		{
			if(data->linestate > 0)
				data->linestate = LINE_HALF_RIGHT;
			else
				data->linestate = LINE_HALF_TRACK_LEFT;
				
			return LINE_LOST;
		}*/
		
		if(position > data->hysteresis_threshold || position < -data->hysteresis_threshold)
		{
			return LINE_LOST;
		}
		//Identifie le bord de piste droit
		else if(data->threshold_image[data->falling_edges_position[0]] == 1)
		{
			if(data->deglitch_counter >= data->deglitch_limit)
			{
				data->linestate = LINE_TRACK_RIGHT;
				data->line_position = position - data->halftrack_width + data->offset;
				return LINE_OK;
			}
			else
			{
				data->deglitch_counter++;
				return LINE_LOST;
			}
			
		}
		else if(data->threshold_image[data->falling_edges_position[0]] == 2)
		{
			if(data->deglitch_counter >= data->deglitch_limit)
			{
				data->linestate = LINE_TRACK_LEFT;
				data->line_position = position + data->halftrack_width + data->offset;
				return LINE_OK;
			}
			else
			{
				data->deglitch_counter++;
				return LINE_LOST;
			}
		}
		else
			return LINE_LOST;
	}
	else if(data->edges_count == 2)
	{
		data->deglitch_counter = 0;
		//Check derivative order 
			
		//1 edge - compute center & record
		position = (data->rising_edges_position[0] + data->falling_edges_position[0] + data->rising_edges_position[1] + data->falling_edges_position[1]) / 4.f;
		position -= 64;
		data->line_position = position + data->offset;
		data->current_linewidth = (data->rising_edges_position[0] + data->falling_edges_position[0]) / 2.f -
						  	  	  (data->rising_edges_position[1] + data->falling_edges_position[1]) / 2.f;
		
		if(data->current_linewidth < 0.f)
			data->current_linewidth *= -1;
		
		data->current_linewidth_diff = (data->current_linewidth - data->linewidth) / data->current_linewidth;
		
		if(data->current_linewidth_diff > 0.4 || data->current_linewidth_diff < -0.4)
			return LINE_LOST;
		
		if(data->line_position > 0.f)
			data->linestate = LINE_LEFT;
		else if(data->line_position < 0.f)
			data->linestate = LINE_RIGHT;
		else
			data->linestate = LINE_CENTER;
		return LINE_OK;
	}/*
	else if(data->edges_count == 6)
	{
		return LINE_LOST;
		//TODO : CHECK INTERVALS ARE CORRECT 
		position = (data->rising_edges_position[2] + data->falling_edges_position[2] + data->rising_edges_position[3] + data->falling_edges_position[3]) / 4.f;
		position -= 64;
		data->line_position = position + data->offset;
		//TODO : CALL FUNCTION TO SIGNAL START LINE
		return LINE_OK;
	}
	else
	{
		return LINE_LOST;
		float linewidth = 0.f;
		float error = 0.f, lowest_error = 1000.f;
		uint16_t valid_index = 0;
		
		//Select most probable line
		for(i = 0 ; i < data->edges_count - 1 ; i++)
		{
			linewidth = (data->rising_edges_position[i] + data->falling_edges_position[i]) / 2.f -
						(data->rising_edges_position[i + 1] + data->falling_edges_position[i + 1]) / 2.f;
			
			error = (linewidth - data->linewidth) / data->linewidth;
			
			if(error < 0.f)
				error *= -1;
			
			if(error < lowest_error)
			{
				lowest_error = error;
				valid_index = i;
			}
		}
		
		if(lowest_error < 0.10)
		{
			//Valid line found, compute position
			position = (data->rising_edges_position[valid_index] + data->falling_edges_position[valid_index] + 
					    data->rising_edges_position[valid_index + 1] + data->falling_edges_position[valid_index + 1]) / 4.f;
			position -= 64;
			data->line_position = position + data->offset;
			return LINE_OK;
		}
		else
		{
			return LINE_LOST;
		}
	}*/
	return LINE_LOST;
}


//Detects erratic behavior of line position, and puts a corrected value in valid_line_position
//Locks line value if changes from left to right are too big
void compute_valid_line_position(cameraData* data, int linestate)
{
	//If the distance between previous and new valid line is > threshold
	data->distance = data->line_position - data->valid_line_position;
	
	if(linestate == LINE_UNSURE)
	{
		if(data->valid_line_position > -20 && data->line_position < 40)
		{
			//Wrong line detected...Max out the error
			data->valid_line_position = 64;
			data->previous_line_position = 1;
		}
		else if(data->valid_line_position < 20 && data->line_position > -40)
		{
			data->valid_line_position = -64;
			data->previous_line_position = -1;
		}
		else
		{
			data->valid_line_position = data->line_position;
			data->previous_line_position = 0;
		}
	}
	else
	{
		data->valid_line_position = data->line_position;
		
	}
	
	
}

void calibrate_data(cameraData* data, uint32_t exposure_time_us)
{
	int counter = 0;
	float div = 1.f;
	float center = 0.f;
	int32_t min = 0,max = 0;
	chrono chr;
	uint16_t i; 
	
	
	reset(&chr);
	while(ms(&chr) < 1000)
	{
		//Wait 1 second
	}
	
	
	data->offset = 0.f;
	reset(&chr);
	chrono chr_exposure;
	reset(&chr_exposure);
	
	float linewidth = 0;
	
	//Record raw camera image 100 times or stop at 5 seconds
	while(counter < 100 && ms(&chr) < 5000)
	{
		if(us(&chr_exposure) > exposure_time_us)
		{
			reset(&chr_exposure);
			if(read_process_data(data) == LINE_OK)
			{
				//Sum for average
				center += data->line_position;
				
				// Sum for line width
				if(data->edges_count == 2)
				{
					//1 edge - compute center & record
					linewidth += (data->rising_edges_position[1] + data->falling_edges_position[1]) / 2.f -
								 (data->rising_edges_position[0] + data->falling_edges_position[0]) / 2.f ;
				}
				
				//Compute thresholds
				if(counter == 0)
				{
					min = data->derivative_image[data->edgeleft];
					max = data->derivative_image[data->edgeleft];
				}
				int16_t i;
				for(i = data->edgeleft ; i < 128 - data->edgeright ; i++)
				{
					if(data->derivative_image[i] < min)
						min = data->derivative_image[i];
					if(data->derivative_image[i] > max)
						max = data->derivative_image[i];
					
					//Compute threshold
					data->threshold = (int32_t)(max / 2.f);
				}
				counter++;
			}
		}
	}
	
	if(counter == 0)
	{
		serial_printf("Calibration step 1 : ISSUE : %d \n",counter);
		TFC_Task();
		counter=1;
	}
	else
		data->linewidth = linewidth;
		
	div = (float)(counter);
	
	//Compute average	
	data->offset = - center / div;
	
	//Compute linewidth
	data->linewidth /= div; 
	
	serial_printf("Calibration step 1 done on &d samples.\n",counter);
	TFC_Task();
	
	reset(&chr);
	while(ms(&chr) < 1000)
	{
		TFC_Task();
		//Wait 1 second
	}
	
	//Wait pushbutton for white calibration
	uint8_t go = 0;
	while(go == 0)
	{
		if(TFC_PUSH_BUTTON_0_PRESSED)
			go++;
	}
	
	
	for(i = data->edgeleft ; i < 128 - data->edgeright ; i++)
	{
		data->derivative_zero[i] = 0;
	}
	
	counter = 0;
	reset(&chr);
	
	//Empty camera sensor
	read_process_data(data);
	reset(&chr_exposure);
	
	//Record raw camera image 100 times or stop at 5 seconds
	while(counter < 200 && ms(&chr) < 5000)
	{
		if(us(&chr_exposure) > exposure_time_us)
		{
			reset(&chr_exposure);
			
			if(read_process_data(data) == LINE_LOST)
			{
				
				// Sum derivative
				if(data->edges_count == 0)
				{
					for(i = data->edgeleft ; i < 128 - data->edgeright ; i++)
					{
						data->derivative_zero[i] += data->derivative_image[i];
					}
					counter++;
				}
			}
			
		}
	}
	
	if(counter == 0)
	{
		serial_printf("Calibration step 2 : ISSUE : %d \n",counter);
		TFC_Task();
		counter = 1;
	}
	//Pourquoi pas besoin de faire ca ??
	/*
	for(i = data->edgeleft ; i < 128 - data->edgeright ; i++)
	{
		data->derivative_zero[i] /= counter;
	}*/
	serial_printf("Calibration step 2 done on %d samples\n",counter);
	TFC_Task();
	
	reset(&chr);
	while(ms(&chr) < 1000)
	{
		TFC_Task();
		//Wait 1 second
	}
}


