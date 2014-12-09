#include "derivative.h" /* include peripheral declarations */
#include "TFC\TFC.h"
#include "camera_processing.h"
#include "serial.h"
#include "serial_protocol.h"
#include "logger.h"

int main(void)

{
	TFC_Init();
	
	cameraData data;
	
	init_data(&data);
	
	//Computation variables
	float position_error = 0.f;
	float previous_error = 0.f;
	float error_derivative = 0.f;
	float error_integral = 0.f;
	float looptime = 0.f;
	float command = 0.f;
	float command_engines = 0.f;
	int engines_on = 1;
	int i;
	
	//Parameters
	float P = 0.01;
	float I = 0.01;
	float D = 0.001;
	
	//Camera processing parameters
	data.threshold_coefficient = 0.65;
	data.edgeleft = 20;
	data.edgeright = 15;
	data.alpha = 0.25;
	
	init_serial_protocol();
	init_log();
	
	//TFC_HBRIDGE_ENABLE;
	
	uint8_t led_state = 0;
	TFC_SetBatteryLED_Level(led_state);
	
	float test[128] = {0};
	
	for(i = 0 ; i < 128 ; i++)
	{
		test[i] = i;
	}

	//Readonly variables
	add_to_log(data.filtered_image,4*128,FLOAT,1,"filtered_line");
	add_to_log(data.raw_image,4*128,FLOAT,1,"raw_line");
	add_to_log(&position_error, 4, FLOAT,1,"error");
	add_to_log(&error_derivative, 4, FLOAT,1,"derivative");
	add_to_log(&error_integral, 4, FLOAT,1,"integral");
	add_to_log(&command,4,FLOAT,1,"command");
	//Read/write variables
	add_to_log(&P,4,FLOAT,0,"P");
	add_to_log(&I,4,FLOAT,0,"I");
	add_to_log(&D,4,FLOAT,0,"D");
	add_to_log(&command_engines,4,FLOAT,0,"engines");
	//Test variables (as reference in case of MCU or GUI malfunction)
	add_to_log(test,4*128,FLOAT,1,"table");
	
	for(;;)
	{	   
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		TFC_Task();
		
		//Logger debug
		if(TFC_Ticker[2] > 100)
		{
			TFC_Ticker[2] = 0;
			
			update_log_serial();
			update_serial_protocol();
			
			for(i = 0 ; i < 128 ; i++)
			{
				test[i] += 1.f;
				if(test[i] > 128.f)
					test[i] = 0;
			}
		}
		
		//Led state
		if(TFC_Ticker[5] > 500)
		{
			TFC_Ticker[5] = 0;
			led_state ^= 0x01; 			
		}
		
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
		}
		
		//Update direction every 100ms
		if(TFC_Ticker[7] > 100)
		{
			TFC_Ticker[7] = 0;
			
			//Compute servo command between -1.0 & 1.0
			command = P * position_error + D * error_derivative + I * error_integral;
			
			if(command > 1.f)
				command = 1.f;
			if(command < -1.f)
				command = -1.f;
			
			TFC_SetServo(0, command);
			
			if(command_engines != 0.f)
			{
				TFC_HBRIDGE_ENABLE;
			}
			else
				TFC_HBRIDGE_DISABLE;
			
			TFC_SetMotorPWM(command_engines , command_engines);
			
		}
		
		//Button events
		if(TFC_PUSH_BUTTON_0_PRESSED)
		{
			calibrate_data(&data);
			error_integral = 0.f;
			led_state = 3;
		}
	
		TFC_SetBatteryLED_Level(led_state);
	}
	return 0;
}
