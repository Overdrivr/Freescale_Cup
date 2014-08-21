#include "derivative.h" /* include peripheral declarations */
#include "TFC\TFC.h"
#include "camera_processing.h"

void imageToSerial(uint16_t* image)
{
	uint16_t i;
	for(i=0;i<128;i++)
	{
		 uint16_t val = image[i];
		 uint32_t val2 = val;
		 TERMINAL_PRINTF("%d ",val2);
		 
	}
	TERMINAL_PRINTF("\n");	
}

void imageToSerial2(int8_t* image)
{
	int16_t i;
	for(i=0;i<128;i++)
	{
		 int8_t val = image[i];
		 int32_t val2 = val;
		 TERMINAL_PRINTF("%d ",val2);
		 
	}
	TERMINAL_PRINTF("\n");	
}

int main(void)
{
	
	
	TFC_Init();
	
	cameraData data;
	
	initData(&data);
	
	/*
	
	uint32_t i=0,j=0;
	uint16_t image[128];
	uint16_t filtered_image[128];
	int8_t processed_image[128];
	uint16_t min, max;
	uint16_t falling_edges[128],rising_edges[128];
	uint16_t edges_count = 0;
	uint8_t edge_signal;
	int buffer;
	float alpha = 0.3;
	
	//Init image buffer
	for(i=0;i<128;i++)
	{
		image[i] = 0;
		filtered_image[i] = 0;
		processed_image[i] = 0;
		falling_edges[i] = 0;
		rising_edges[i] = 0;
	}
	*/
	for(;;)
	{	   
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		TFC_Task();
		
		if(readNProcessData(&data,0.3))
		{
			//Output image to serial
			//imageToSerial2(data.threshold_image);
			//imageToSerial(data.filtered_image);
			
			//Output line position
			TERMINAL_PRINTF("%d\n",data.line_position);
		}
		
		/*			
		if(TFC_Ticker[0]>100 && LineScanImageReady==1)
		{
			TFC_Ticker[0] = 0;
			LineScanImageReady=0;
			
			//Record current image
			for(i=0;i<128;i++)
			{
				 image[i] = LineScanImage1[i];				 
			}
			
			//Simple complementary filter
			for(i=0;i<128;i++)
			{
				filtered_image[i] = filtered_image[i] * alpha + image[i] * (1.0 - alpha);
			}
			
			//Min Max detection
			min = 65535;
			max = 0;
			for(i=0;i<128;i++)
			{
				if(filtered_image[i] > max)
					max = filtered_image[i];
				if(filtered_image[i] < min)
					min = filtered_image[i];
			}
			
			//TERMINAL_PRINTF("%d ",max - min);
			
			//Adjust dynamic, remove offset and apply threshold
			//TODO : ALSO SET TO 1 PIXEL THAT ARE TOO BRIGHT TO BE THE LINE
			for(i=0;i<128;i++)
			{
				buffer = filtered_image[i];
				buffer = (buffer - min) * 1000 / (max - min);
				
				if(buffer > 750 || 
				   i < 15 || 
				   i > (128 - 15))
					processed_image[i] = 1;
				else
					processed_image[i] = -1;
			}
			
			//Look for falling then rising edge, from pixel 0 to 128
			edges_count = 0;
			edge_signal = 0;
			for(i=0;i<127;i++)
			{
				//Edge detected
				if(processed_image[i] * processed_image[i+1] < 0)
				{
					//First edge detected (edges go by pair)
					if(edge_signal == 0)
					{
						edge_signal = 1;
						//Record position
						falling_edges[edges_count] = i;
					}
					else
					{
						edge_signal = 0;
						//Record position
						rising_edges[edges_count] = i;
						edges_count++;
					}
				}
				
			}
			
			//TERMINAL_PRINTF("%d --------------- \n",edges_count);
			
			//Filter edges less wide than XX pixel
			for(i = 0 ; i < edges_count ; i++)
			{
				buffer = rising_edges[i];
				buffer -= falling_edges[i];
					//TERMINAL_PRINTF("B = %d\n",buffer);
				if(buffer < 20)
				{
					//Remove all edges between both points
					for(j = falling_edges[i] ; j <= rising_edges[i] ; j++)
					{
						processed_image[j] = 1;
						
					}
				}
				
			}*/
		
			
			
			
		//}
	}
	
	return 0;
}
