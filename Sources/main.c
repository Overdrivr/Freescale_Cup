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
	int val = 0.f;
	float command = 0.f;
	float command_engines = 0.f;
	int engines_on = 1;
	
	//Parameters
	float P = 0.01;
	float I = 0.01;
	float D = 0.001;
	
	//Camera processing parameters
	data.threshold_coefficient = 0.65;
	data.edgeleft = 20;
	data.edgeright = 15;
	data.alpha = 0.25;
	
	int32_t test = 0;
	init_serial_protocol();
	log log_table;
	init_log(&log_table);
	add_to_log(&log_table,&test, 4, INT32,1,"test_var");
	
	TFC_HBRIDGE_ENABLE;
	
	uint8_t led_state = 0;
	TFC_SetBatteryLED_Level(led_state);
	
	for(;;)
	{	   
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		TFC_Task();
		
		//Logger debug
		if(TFC_Ticker[2] > 300)
		{
			TFC_Ticker[2] = 0;
			update_log_serial(&log_table);
			test++;
			if(test > 128)
				test = -128;
			update_serial_protocol();
			serial_printf("TX");
			serial_printf("\n");
		}
		
		//Led state
		/*if(TFC_Ticker[5] > 500)
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
			//TERMINAL_PRINTF("%d ", val);
			//TERMINAL_PRINTF("\n");
		}
		
		//Button events
		if(TFC_PUSH_BUTTON_0_PRESSED)
		{
			calibrate_data(&data);
			error_integral = 0.f;
			led_state = 3;
		}*/
		
		/*if(TFC_PUSH_BUTTON_1_PRESSED & TFC_Ticker[6] > 500)
		{
			TFC_Ticker[6] = 0;
			//Start-Stop engines
			if(engines_on == 0)
			{
				engines_on = 1;
				TFC_HBRIDGE_ENABLE;//?
				TFC_BAT_LED3_TOGGLE;
			}	
			else
			{
				engines_on = 0;
				TFC_HBRIDGE_DISABLE;//?
				TFC_BAT_LED3_TOGGLE;
			}
				
		}*/
		/*
		if(engines_on == 0)
		{
			TFC_SetMotorPWM(0.f , 0.f);
		}
		else
		{
			command_engines =  TFC_ReadPot(0);
			TFC_SetMotorPWM(command_engines , command_engines);
			command_engines *= 1000;
		}
		
		//send_serial_frame(&command_engines,2);
	
		TFC_SetBatteryLED_Level(led_state);*/
	}
	return 0;
}
