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
	}
	data->edges_count = 0;
	data->line_position = 0;
	data->valid_line_position = 0;
	data->image_integral = 0;//Compute reference integral at calibration
	
	data->threshold = 200;
	data->offset = 0.f;
	data->linewidth = 0.f;
	
	data->edgeleft = 1;//MIN VALUE : 1
	data->edgeright = 1;//MIN VALUE : 1
	
}


int read_process_data(cameraData* data)
{	
	float position;
	uint8_t edge_signal;
	int16_t i;
	int16_t loopright = 128 - data->edgeright;
		
	data->image_integral = 0;
	
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
	if(data->image_integral < data->linewidth * data->threshold * 0.5)
		return LINE_LOST;
	
	data->edges_count = 0;
	edge_signal = 0;
	data->derivative_image[data->edgeleft] = data->raw_image[data->edgeleft];
	data->threshold_image[data->edgeleft-1] = -1;
	data->threshold_image[loopright] = -1;
	
	for(i = data->edgeleft ; i < loopright ; i++)
	{
		if(i < 127)
			data->derivative_image[i] =  data->raw_image[i+1] -  data->raw_image[i];
		
		//Apply threshold
		if(data->derivative_image[i] >  data->threshold || 
		   data->derivative_image[i] < -data->threshold)
		{
			data->threshold_image[i] = 1;
		}
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
		//Line is in very border of camera, half of it is visible
		//1 edge - compute center & record
		position = (float)(data->rising_edges_position[0] + data->falling_edges_position[0]) / 2.f;
		
		if(position > 64)
			position -= (64 - data->linewidth / 2.f);
		else
			position -= (64 + data->linewidth / 2.f);
		
		data->line_position = position + data->offset;
	}
	else if(data->edges_count == 2)
	{
		//1 edge - compute center & record
		position = (data->rising_edges_position[0] + data->falling_edges_position[0] + data->rising_edges_position[1] + data->falling_edges_position[1]) / 4.f;
		position -= 64;
		data->line_position = position + data->offset;
	}
	else if(data->edges_count == 6)
	{
		//TODO : CHECK INTERVALS ARE CORRECT 
		position = (data->rising_edges_position[2] + data->falling_edges_position[2] + data->rising_edges_position[3] + data->falling_edges_position[3]) / 4.f;
		position -= 64;
		data->line_position = position + data->offset;
		//TODO : CALL FUNCTION TO SIGNAL START LINE
	}
	else
	{
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
		}
		else
		{
			return LINE_LOST;
		}
	}
	return LINE_OK;
}


//Detects erratic behavior of line position, and puts a corrected value in valid_line_position
//Locks line value if changes from left to right are too big
uint8_t compute_valid_line_position(cameraData* data)
{
	//If the distance between previous and new valid line is > 50
	int16_t distance = data->valid_line_position - data->line_position;
	if(distance > 50 || distance < -50)
		return 1;
	
	data->valid_line_position = data->line_position;
	return 0;
}

void calibrate_data(cameraData* data)
{
	int counter = 0;
	float div = 1.f;
	float center = 0.f;
	int32_t min,max;
	
	chrono chr;
	
	Restart(&chr);
	
	
	while(GetLastDelay_ms(&chr) < 1000)
	{
		Capture(&chr);
		//Wait 1 second
	}
	
	
	Restart(&chr);
	
	data->offset = 0.f;
	data->linewidth = 0.f;
	
	//Record raw camera image 100 times or stop at 5 seconds
	while(counter < 100 && GetLastDelay_ms(&chr) < 5000)
	{
		Capture(&chr);
		if(read_process_data(data) == LINE_OK)
		{
			//Sum for average
			center += data->line_position;
			counter++;
			
			// Sum for line width
			if(data->edges_count == 2)
			{
				//1 edge - compute center & record
				data->linewidth += (data->rising_edges_position[1] + data->falling_edges_position[1]) / 2.f -
								   (data->rising_edges_position[0] + data->falling_edges_position[0]) / 2.f ;
			}
			else
			{
				serial_printf("CALIBRATION_ISSUE");
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
				//TO FIX : ISSUES WITH CALIBRATION !!
				data->threshold = (int32_t)(max / 2.f);
			}			
		}
		else
		{
			serial_printf("CALIBRATION_ISSUE 2");
		}
					
	}	
	
	if(counter == 0)
		return;
	
	div = (float)(counter);
	
	//Compute average	
	data->offset = - center / div;
	
	//Compute linewidth
	data->linewidth /= div; 
}

