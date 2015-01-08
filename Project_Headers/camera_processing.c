/*
 * camera_processing.c
 *
 *  Created on: Aug 19, 2014
 *      Author: B48923
 */

#include "camera_processing.h"
#include "chrono.h"

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
	}
	data->edges_count = 0;
	data->line_position = 0;
	data->valid_line_position = 0;
	
	data->threshold = 1000.f;
	data->threshold_coefficient = 0.65;
	data->edgeleft = 20;
	data->edgeright = 15;
	data->alpha = 0.25;
	data->offset = 0;
}


int read_process_data(cameraData* data)
{	
	uint16_t min, max;
	float val;
	float position;
	uint8_t edge_signal;
	int16_t i;
	int16_t loopright = 128 - data->edgeright;

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
	
	//Left-starting complementary filter
	data->filtered_image[data->edgeleft] = data->raw_image[data->edgeleft];
	
	for(i = data->edgeleft ; i < loopright ; i++)
	{
		val = data->raw_image[i+1];
		data->filtered_image[i+1] = data->filtered_image[i] * (1 - data->alpha) + val * data->alpha;
	}
	
	
	//Apply threshold
	for(i = data->edgeleft ; i < loopright ; i++)
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
	for(i = data->edgeleft ; i < loopright ; i++)
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
		//TODO : Monitor when loop goes there, many times it seems
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
	return 0;
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
	
	chrono chr;
	
	Restart(&chr);
	
	
	while(GetLastDelay_ms(&chr) < 1000)
	{
		Capture(&chr);
		//Wait 1 second
	}
	
	
	Restart(&chr);
	
	data->offset = 0.f;
	
	//Record raw camera image 20 times or stop at 5 seconds
	while(counter < 20 && GetLastDelay_ms(&chr) < 5000)
	{
		Capture(&chr);
		if(read_process_data(data))
		{
			center += data->line_position;
			counter++;
		}
	}
	
	
	
	if(counter == 0)
		return;
	
	div = (float)(counter);
	
	//Compute average
	center /= div;
	
	data->offset = -(int16_t)(center);
}

