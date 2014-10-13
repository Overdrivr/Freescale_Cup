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
		data->filtered_image2[i] = 0;
		data->derivate_image[i] = 0;
		data->threshold_image[i] = 0;
		data->falling_edges_position[i] = 0;
		data->rising_edges_position[i] = 0;
		data->threshold_filtered_image[i] = 0.f;
		
		data->raw_img[i] = 0.f;
		data->d1_img[i] = 0.f;
		data->d2_img[i] = 0.f;
	}
	data->edges_count = 0;
	data->line_position = 0;
}


int readNProcessData(cameraData* data, float tau, float timestep)
{	
	float min, max;
	uint8_t edge_signal;
	float x1,x2;
	int i;
	float looptime = timestep / 1000.f;
	
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
		int edgeleft = 20;
		int edgeright = 15;
		
		for(i=0;i<edgeleft;i++)
		{
			data->raw_image[i] = max;
		}
		
		for(i=(128-edgeright);i<128;i++)
		{
			data->raw_image[i] = max;
		}
		
		
		
		//Second order complementary filter		
		float beta = 1.2f;
		
		for(i=0;i<128;i++)
		{
			data->raw_img[i] = data->raw_image[i];
			
			x1 = (data->raw_img[i] - data->d2_img[i]) * tau * tau;
			data->d1_img[i] = looptime * x1 + data->d1_img[i];
			
			x2 = data->d1_img[i] + (data->raw_img[i] - data->d2_img[i]) * beta * tau;
			
			data->d2_img[i] = looptime * x2 + data->d2_img[i];
		} 
		/*
		x1 = (newAngle -   x_angle2C)*k*k;
		y1 = dtc2*x1 + y1;
		x2 = y1 + (newAngle -   x_angle2C)*2*k + newRate;
		x_angle2C = dtc2*x2 + x_angle2C;
		*/	
			
		//Adjust dynamic, remove offset and apply threshold
		//TODO : ALSO SET TO 1 PIXEL THAT ARE TOO BRIGHT TO BE THE LINE ?
		float threshold = 0.65;
		for(i=0;i<128;i++)
		{
			if(data->d2_img[i] > threshold * (max - min))
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
		
		float alpha = 0.45;
		float t1,t2;
		//Filter threshold image
		for(i=0;i<127;i++)
		{
			t2 = data->threshold_image[i];
			data->threshold_filtered_image[i] = data->threshold_filtered_image[i] * alpha + t2 * 10 * (1 - alpha);
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
		/*
		//Mark position on signal
		for(i=0;i<127;i++)
		{
			data->threshold_image[i] = 0;
		}
		i = data->line_position + 64;
		data->threshold_image[i] = 1;*/  
		
	
	return 1;
	}
	return 0;
}

