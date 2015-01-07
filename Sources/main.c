#include "derivative.h" /* include peripheral declarations */
#include "TFC\TFC.h"
#include "camera_processing.h"
#include "Serial\serial.h"
#include "DistantIO\distantio.h"
#include "chrono.h"

//TODO : Select servo offset with potard
// WARNING : SysTick frequency is 50kHz. Will overflow after roughly 2.5 hours
// CHeck : TFC_SetLineScanExposureTime, read process data

int main(void)

{
	TFC_Init();
	
	cameraData data;
	
	init_data(&data);
	
	//Computation variables
	float position_error = 0.f, alpha_error = 0.8;
	float previous_error = 0.f;
	float error_derivative = 0.f, new_derivative = 0.f, alpha_deriv = 0.85;
	
	float command = 0.f,new_command = 0.f, alpha_command = 0.0;
	float servo_offset = 0.05;
	float command_engines = 0.5f;
	
	float commandD = 0.f;
	float commandP = 0.f;
	
	/* NEW VALUES
	 * 
	 * V = 0.4
	 * P = 
	 * D = 0
	 */
	
	//Parameters
	
	//Stable a 0.40
	//float P = 0.013;
	//float D = 0.0005f;
	
	//Stable a 0.45
	//float P = 0.011;
	//float D = 0.00035f;
	
	//a 0.45
	float P = 0.01;
	float D = 0.00015f;
		
	//To check loop times
	chrono chr_cam_m, chr_loop_m;
	//To ensure loop times
	chrono chr_distantio,chr_cam;
	float t_cam = 0, t_loop = 0;
	float looptime_cam;
	
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
	
	//Readonly variables
	register_scalar(&t_cam,FLOAT,0,"cam time(ms)");
	register_scalar(&t_loop,FLOAT,0,"main time(ms)");
	register_scalar(&looptime_cam,FLOAT,0,"cam exe period(us)");

	register_scalar(&position_error, FLOAT,0,"error");
	register_scalar(&error_derivative, FLOAT,0,"derivative");
	register_scalar(&command,FLOAT,0,"command");
	register_scalar(&commandP,FLOAT,0,"cmdP");
	register_scalar(&commandD,FLOAT,0,"cmdD");
	
	//Read/write variables
	register_scalar(&P,FLOAT,1,"P");
	register_scalar(&D,FLOAT,1,"D");
	register_scalar(&alpha_error,FLOAT,1,"alpha P");
	register_scalar(&alpha_deriv,FLOAT,1,"alpha_D");
	register_scalar(&alpha_command,FLOAT,1,"alpha cmd");
	register_scalar(&command_engines,FLOAT,1,"engines");
	register_scalar(&servo_offset,FLOAT,1,"servo_offset");
	
	register_scalar(&data.offset,INT16,0,"offset");
	
	//add_to_log(&looptime,4,FLOAT,1,"looptime");
	//add_to_log(&chr_main.duration,4,UINT32,1,"main loop");
	
	register_array(data.filtered_image,128,FLOAT,1,"filtered_line");
	register_array(data.raw_image,128,UINT16,1,"raw_line");
	//add_to_log(data.calibration_data,4*128,FLOAT,1,"cal_data");
	
	/*TICKERS
	0 : read line data
	1 : servo update
	2 : logger update
	3 : led update
	4 : line calibration
	
	6 : chronometers
	7 : test chrono
	*/
	//Tickers are in 1/10 ms now
	float exposure_time_us = 10000;
	uint32_t exposure_time_ms = exposure_time_us / 1000;
	uint32_t servo_update_ms = 200;
	
	TFC_SetLineScanExposureTime(exposure_time_us);
	TFC_SetServo(0, servo_offset);
	
	Restart(&chr_distantio);
	
	for(;;)
	{
		Restart(&chr_loop_m);	//**TIME MONITORING
		
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		TFC_Task();
		
		
		Capture(&chr_distantio);
		if(GetLastDelay_ms(&chr_distantio) > 4)
		{
			TFC_Ticker[2] = 0;
			Restart(&chr_distantio);
			
			t_loop = GetLastDelay_ms(&chr_loop_m);
			t_cam = GetLastDelay_ms(&chr_cam_m);
			
			update_distantio();
		}
		/*
		TFC_Ticker[5] = 0;
		while(TFC_Ticker[5]<100)
		{
			
		}
		*/
			
		
			
		
		//Compute line position
		Capture(&chr_cam);
		looptime_cam = GetLastDelay_us(&chr_cam);
		if(looptime_cam > exposure_time_us)
		{
			Restart(&chr_cam);
			
			
			Restart(&chr_cam_m);	//TIME MONITORING
			//TOCHECK
			read_process_data(&data,exposure_time_ms);			
			
			//Compute errors for PD
			previous_error = position_error;
			position_error =  position_error * alpha_error + data.line_position * (1 - alpha_error);
			
			new_derivative = (position_error - previous_error) * 1000.f / looptime_cam;
			error_derivative = error_derivative * alpha_deriv + (1 - alpha_deriv) * new_derivative;
			
			commandP = P * position_error;
			commandD = D * error_derivative;
			
			//Compute servo command between -1.0 & 1.0
			new_command = commandD + commandP + servo_offset;
			command = command * alpha_command + (1 - alpha_command) * new_command;
			
			if(command > 1.f)
				command = 1.f;
			if(command < -1.f)
				command = -1.f;
			
			Capture(&chr_cam_m); //TIME MONITORING
		}
		
		
		//Update servo command
		if(TFC_Ticker[1] > servo_update_ms)
		{
			TFC_Ticker[1] = 0;
			
			TFC_SetServo(0, command);
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
			TFC_SetMotorPWM(-command_engines , -command_engines);
		}
	
		TFC_SetBatteryLED_Level(led_state);
		
		Capture(&chr_loop_m);	//**TIME MONITORING
	}
	return 0;
}

