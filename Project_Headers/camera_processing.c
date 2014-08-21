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


int readNProcessData(cameraData* data, float alpha)
{	
	uint16_t min, max;
	uint8_t edge_signal;
	int i,j;
	int buffer;
	
	if(TFC_Ticker[0]>100 && LineScanImageReady==1)
	{
		TFC_Ticker[0] = 0;
		LineScanImageReady = 0;
		
		//Record current image
		for(i=0;i<128;i++)
		{
			data->raw_image[i] = LineScanImage1[i];				 
		}
		
		//Simple complementary filter
		for(i=0;i<128;i++)
		{
			data->filtered_image[i] = data->filtered_image[i] * alpha + data->raw_image[i] * (1.0 - alpha);
		}
		
		//Min Max detection
		min = 65535;
		max = 0;
		for(i=0;i<128;i++)
		{
			if(data->filtered_image[i] > max)
				max = data->filtered_image[i];
			if(data->filtered_image[i] < min)
				min =data->filtered_image[i];
		}
			
		//Adjust dynamic, remove offset and apply threshold
		//TODO : ALSO SET TO 1 PIXEL THAT ARE TOO BRIGHT TO BE THE LINE ?
		for(i=0;i<128;i++)
		{
			buffer = data->filtered_image[i];
			buffer = (buffer - min) * 1000 / (max - min);
			
			if(buffer > 750 || 
			   i < 15 || 
			   i > (128 - 15))
				data->threshold_image[i] = 1;
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
			
		//Filter edges less wide than XX pixel
		for(i = 0 ; i < data->edges_count ; i++)
		{
			buffer = data->rising_edges_position[i];
			buffer -= data->falling_edges_position[i];
			if(buffer < 20)
			{
				//Remove all edges between both points
				for(j = data->falling_edges_position[i] ; j <= data->rising_edges_position[i] ; j++)
				{
					data->threshold_image[j] = 1;
					
				}
			}
			
		}
		
		//Either we have one edge (normal behavior) or three
		if(data->edges_count == 1)
		{
			buffer = (data->rising_edges_position[0] + data->falling_edges_position[0]) / 2;
			buffer -= 64;
			data->line_position = buffer;
		}
		else if(data->edges_count == 3)
		{
			buffer = (data->rising_edges_position[1] + data->falling_edges_position[1]) / 2;
			buffer -= 64;
			data->line_position = buffer;
			//TODO : CALL FUNCTION TO SIGNAL START LINE
		}
		//Any other value is error
		else
		{
			//FOR NOW : DO NOT CHANGE THE VALUE
		}
	
	return 1;
	}
	return 0;
}

