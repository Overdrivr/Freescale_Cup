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


void imageToSerial16(int16_t* image)
{
	int16_t i;
	for(i=0;i<128;i++)
	{
		 int16_t val = image[i];
		 int32_t val2 = val;
		 TERMINAL_PRINTF("%d ",val2);
		 
	}
	TERMINAL_PRINTF("\n");	
}

void imageToSerialf(float* image)
{
	int16_t i;
	for(i=0;i<128;i++)
	{
		 int val = image[i];
		 TERMINAL_PRINTF("%d ",val);
		 
	}
	TERMINAL_PRINTF("\n");	
}

int main(void)
{
	
	
	TFC_Init();
	
	cameraData data;
	
	initData(&data);
	
	float position_error = 0.f;
	float previous_error = 0.f;
	float error_derivative = 0.f;
	float error_integral = 0.f;
	
	for(;;)
	{	   
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		TFC_Task();
		
		//Compute line position
		if(readNProcessData(&data,10,50))
		{
			//Output image to serial
			imageToSerial2(data.threshold_image);
			//imageToSerialf(data.d3_img);
			//imageToSerialf(data.threshold_filtered_image);
			//imageToSerial16(data.derivate_image);
			
			
			position_error = data.line_position;
			//Compute stuff
		}
		
		//Update direction every 500us
		if(TFC_Ticker[1] > 500 )
		{
			TFC_Ticker[1] = 0;
			
			//Compute servo command between -1.0 & 1.0
			
		
		}
	}
	
	return 0;
}
