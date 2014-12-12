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
	float command_engines = -0.4f;
	int i;
	float commandD = 0.f;
	float commandI = 0.f;
	float commandP = 0.f;
	
	//Parameters
	float P = 0.01;
	float I = 0.f;
	float D = 0.f;
	
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
	add_to_log(data.raw_image,4*128,UINT16,1,"raw_line");
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
	
	/*TICKERS
	0 : read line data
	1 : servo update
	2 : logger update
	3 : led update
	4 : line calibration
	5 : PID loop time
	*/
	uint32_t exposure_time_us = 10000;
	uint32_t exposure_time_ms = 10;
	uint32_t servo_update_ms = 20;
	
	TFC_SetLineScanExposureTime(exposure_time_us);

	
	for(;;)
	{	   
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		TFC_Task();
		
		//Compute line position
		if(read_process_data(&data,exposure_time_ms))
		{
			//TODO : Check this loop runs at minimal time 
			//Compute looptime
			looptime = TFC_Ticker[5];
			TFC_Ticker[5] = 0;
			
			//Compute errors for PID
			previous_error = position_error;
			position_error = data.line_position;
			error_derivative = (position_error - previous_error) * 1000.f / looptime;
			error_integral = error_integral + position_error * looptime / 1000.f;
			
			commandP = P * position_error;
			commandI = I * error_integral;
			commandD = D * error_derivative;
			
			//Compute servo command between -1.0 & 1.0
			command = commandD + commandI + commandP;
			
			if(command > 1.f)
				command = 1.f;
			if(command < -1.f)
				command = -1.f;
		
		}
		
		//Update servo command
		if(TFC_Ticker[1] > servo_update_ms)
		{
			TFC_Ticker[1] = 0;
			
			TFC_SetServo(0, command);
		}
		
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
		if(TFC_Ticker[3] > 500)
		{
			TFC_Ticker[3] = 0;
			led_state ^= 0x01; 			
		}
		
		//Calibrate
		if(TFC_PUSH_BUTTON_0_PRESSED)
		{
			calibrate_data(&data,exposure_time_ms);
			error_integral = 0.f;
			led_state = 3;
		}
		
		if(TFC_GetDIP_Switch() == 0)
		{
			TFC_HBRIDGE_DISABLE;
			TFC_SetMotorPWM(0 , 0);
		}
		else
		{
			TFC_HBRIDGE_ENABLE;
			TFC_SetMotorPWM(command_engines , command_engines);
		}
	
		TFC_SetBatteryLED_Level(led_state);
	}
	return 0;
}
