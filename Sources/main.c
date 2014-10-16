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
	
	//Computation variables
	float position_error = 0.f;
	float previous_error = 0.f;
	float error_derivative = 0.f;
	float error_integral = 0.f;
	float looptime = 0.f;
	int val = 0.f;
	float command = 0.f;
	float command_engines = 0.f;
	int engines_on = 0;
	
	//Parameters
	float P = 0.01;
	float I = 0.01;
	float D = 0.001;
	
	//Camera processing parameters
	data.threshold_coefficient = 0.65;
	data.edgeleft = 20;
	data.edgeright = 15;
	data.alpha = 0.25;
	
	TFC_HBRIDGE_DISABLE;
	
	for(;;)
	{	   
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		TFC_Task();
				
		
		//Compute line position
		if(read_process_data(&data))
		{
			//Compute looptime
			looptime = TFC_Ticker[1];
			TFC_Ticker[1] = 0;
			
			//Compute errors for PID
			previous_error = position_error;
			position_error = data.line_position;
			error_derivative = (position_error - previous_error) * 1000.f / looptime;
			error_integral = error_integral + position_error * looptime / 1000.f;
			
			
			//Debug - output to serial
			//val = position_error;
			//val = error_integral;
			//val = error_derivative;
			//TERMINAL_PRINTF("%d ",val);
			//TERMINAL_PRINTF("\n");
			
			//Output image to serial
			//imageToSerialf(data.calibration_data);
			//imageToSerialf(data.filtered_image);
			//imageToSerial2(data.threshold_image);
			//imageToSerial16(data.derivate_image);
		}
		
		//Update direction every 100ms
		if(TFC_Ticker[2] > 100)
		{
			TFC_Ticker[2] = 0;
			
			//Compute servo command between -1.0 & 1.0
			command = P * position_error; //+ I * error_integral;
			
			if(command > 1.f)
				command = 1.f;
			if(command < -1.f)
				command = -1.f;
			
			TFC_SetServo(0, command);
			
			val = command * 1000;
			TERMINAL_PRINTF("%d ", val);
			TERMINAL_PRINTF("\n");
		}
		
		//Button events
		if(TFC_PUSH_BUTTON_0_PRESSED)
		{
			calibrate_data(&data);
			error_integral = 0.f;
		}
		if(TFC_PUSH_BUTTON_1_PRESSED)
		{
			//Start-Stop engines
			if(engines_on == 0)
			{
				engines_on = 1;
				TFC_HBRIDGE_ENABLE;//?
			}	
			else
			{
				engines_on = 0;
				TFC_HBRIDGE_DISABLE;//?
			}
				
		}
		
		if(engines_on == 0)
		{
			TFC_SetMotorPWM(0.f , 0.f);
		}
		else
		{
			command_engines =  TFC_ReadPot(0);
			TFC_SetMotorPWM(command_engines , command_engines);
		}
	}
	
	return 0;
}
