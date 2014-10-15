/*
 * camera_processing.c
 *
 *  Created on: Aug 19, 2014
 *      Author: B48923
 */

#include "camera_processing.h"

void initData(cameraData* data)
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
}


int readNProcessData(cameraData* data)
{	
	float min, max, val;
	uint8_t edge_signal;
	uint16_t i;
	
	//Parameters
	float threshold = 0.65;
	int edgeleft = 20;
	int edgeright = 15;
	float alpha = 0.25;
	
	if(TFC_Ticker[0]>100 && LineScanImageReady==1)
	{
		TFC_Ticker[0] = 0;
		LineScanImageReady = 0;
		
		//Record current image
		for(i=0;i<128;i++)
		{
			data->raw_image[i] = LineScanImage1[i];				 
		}
						
		//Min Max detection
		min = 65535;
		max = 0;
		for(i=0;i<128;i++)
		{
			if(data->raw_image[i] > max)
				max = data->raw_image[i];
			if(data->raw_image[i] < min)
				min =data->raw_image[i];
		}
		
		//Remove edges
		
		
		for(i=0;i<edgeleft;i++)
		{
			data->raw_image[i] = max;
		}
		
		for(i=(128-edgeright);i<128;i++)
		{
			data->raw_image[i] = max;
		}
		
		//Left-starting filter
		data->filtered_image[0] = data->raw_image[0];
		for(i=0;i<127;i++)
		{
			val = data->raw_image[i+1];
			data->filtered_image[i+1] = data->filtered_image[i] * (1-alpha) + val * alpha;
		}
		
		//Min Max on filter values
		min = 65535;
		max = 0;
		for(i=0;i<128;i++)
		{
			if(data->filtered_image[i] > max)
				max = data->filtered_image[i];
			if(data->filtered_image[i] < min)
				min =data->filtered_image[i];
		}
		
		
		//Apply threshold
		for(i=0;i<128;i++)
		{
			if(data->filtered_image[i] > threshold * (max - min))
			{
				//data->d2_img[i] = max;
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
					
		
		float position;
		
		if(data->edges_count == 0)
		{
			//No edge detected, keep line position
		}
		else if(data->edges_count == 1)
		{
			//1 edge - compute center & record
			position = (data->rising_edges_position[0] + data->falling_edges_position[0]) / 2;
			position -= 64;
			data->line_position = position;
		}
		else if(data->edges_count == 3)
		{			
			position = (data->rising_edges_position[1] + data->falling_edges_position[1]) / 2;
			position -= 64;
			data->line_position = position;
			//TODO : CALL FUNCTION TO SIGNAL START LINE
		}
		//Any other value is error
		else
		{
			//FOR NOW : DO NOT CHANGE THE VALUE
		}
		
		//Mark position on signal
		for(i=0;i<128;i++)
		{
			data->threshold_image[i] = 0;
		}
		i = data->line_position + 64;
		data->threshold_image[i] = 1; 
		
	
		return 1;
	}
	return 0;
}

