/*
 * camera_processing.c
 *
 *  Created on: Aug 19, 2014
 *      Author: B48923
 */

#include "camera_processing.h"

void init_data(cameraData* data)
{
	int i;
	//Init image buffer
	for(i=0;i<128;i++)
	{
		data->raw_image[i] = 0;
		data->filtered_image[i] = 0;
		data->threshold_image[i] = 0;
		data->falling_edges_position[i] = 0;
		data->rising_edges_position[i] = 0;
		data->calibration_data[i] = 0.f;
	}
	data->edges_count = 0;
	data->line_position = 0;
	
	data->threshold = 1000.f;
	data->threshold_coefficient = 0.65;
	data->edgeleft = 20;
	data->edgeright = 15;
	data->alpha = 0.25;
	data->offset = 0;
}


int read_process_data(cameraData* data, uint32_t update_delay_ms)
{	
	uint16_t min, max;
	float val;
	float position;
	uint8_t edge_signal;
	uint16_t i;
	
	if(TFC_Ticker[0] < update_delay_ms)
		return 0;
	
	TFC_Ticker[0] = 0;
		
	//Record current image & detect min max
	min = 65535;
	max = 0;
		
	for(i=0;i<128;i++)
	{
		data->raw_image[i] = LineScanImage1[i];
		
		if(data->raw_image[i] > max)
			max = data->raw_image[i];
		if(data->raw_image[i] < min)
			min =data->raw_image[i];
	}
					
	//Remove edges
	for(i = 0 ; i < data->edgeleft ; i++)
	{
		data->raw_image[i] = max;
	}
	
	for(i = (128 - data->edgeright) ; i < 128 ; i++)
	{
		data->raw_image[i] = max;
	}
	
	//Left-starting complementary filter
	data->filtered_image[0] = data->raw_image[0];
	for(i=0;i<127;i++)
	{
		val = data->raw_image[i+1];
		data->filtered_image[i+1] = data->filtered_image[i] * (1 - data->alpha) + val * data->alpha;
	}	
	
	//Apply threshold
	for(i=0;i<128;i++)
	{
		if(data->filtered_image[i] > data->threshold)
		{
			data->threshold_image[i] = 1;
		}
		else
			data->threshold_image[i] = -1;
	}
		
	//Look for falling then rising edge, from pixel 0 to 128
	data->edges_count = 0;
	edge_signal = 0;
	for(i=0;i<127;i++)
	{
		//Edge detected
		if(data->threshold_image[i] * data->threshold_image[i+1] < 0)
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
	}
	else if(data->edges_count == 1)
	{
		//1 edge - compute center & record
		position = (data->rising_edges_position[0] + data->falling_edges_position[0]) / 2;
		position -= 64;
		data->line_position = position + data->offset;
	}
	else if(data->edges_count == 3)
	{			
		position = (data->rising_edges_position[1] + data->falling_edges_position[1]) / 2;
		position -= 64;
		data->line_position = position + data->offset;
		//TODO : CALL FUNCTION TO SIGNAL START LINE
	}
	//Any other value is error
	else
	{
		//FOR NOW : DO NOT CHANGE THE VALUE
	}
	
	return 1;
}

void calibrate_data(cameraData* data, uint32_t update_delay_ms)
{
	
	int i = 0, counter = 0;
	float min, max, val;
	
	TFC_Ticker[4] = 0; 
	
	for(i=0;i<128;i++)
	{
		data->calibration_data[i] = 0;
	}
	
	//Record raw camera image 20 times or stop at 5000 seconds
	while(counter < 20 && TFC_Ticker[4] < 5000)
	{
		if(TFC_Ticker[0] > update_delay_ms)
		{
			TFC_Ticker[0] = 0;
			
			for(i=0;i<128;i++)
			{
				data->calibration_data[i] += LineScanImage1[i];
			}
			counter++;
		}
	}
	
	//Compute average
	for(i=0;i<128;i++)
	{
		data->calibration_data[i] /= (1.0f * counter);
	}
	
	//Min Max detection
	min = data->calibration_data[0];
	max = data->calibration_data[0];
	
	for(i = data->edgeleft ; i < 128 - data->edgeright ; i++)
	{
		if(data->calibration_data[i] > max)
			max = data->calibration_data[i];
		if(data->calibration_data[i] < min)
			min = data->calibration_data[i];
	}		
	
	//Compute fixed threshold level
	data->threshold = data->threshold_coefficient * (max - min) + min;
}

