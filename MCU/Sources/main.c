#include "derivative.h" /* include peripheral declarations */
#include "TFC/TFC.h"
#include "camera_processing.h"
#include "DistantIO/distantio.h"
#include "chrono.h"
#include "UnitTests/UnitTests.h"
#include "TFC/TFC_UART.h"

//TODO : Investiger erreurs avec virage+discontinuites

// WARNING : SysTick frequency is 50kHz. Will overflow after roughly 2.5 hours
// Derivative not clean

void cam_program();
void configure_bluetooth();

float max(float a, float b)
{
	if(a>b)
		return a;
	return b;
}


int main(void)

{
	//test_serial1();
	//test_protocol1();
	//test_distantio_minimal();
	
	cam_program();
	
	//configure_bluetooth();
	
	return 0;
}

//TODO : Calibrer offset

void cam_program()
{
	TFC_Init();
	//Init all 3 communication layers
	init_serial();
	init_protocol();
	init_distantio();
	
	cameraData data;
	
	init_data(&data);
	
	//Computation variables
	float position_error = 0.f;
	float previous_error = 0.f;
	float error_derivative = 0.f;
	float alpha_error = 0.85;
	float filtered_error = 0.f;
	
	float command = 0.f;
	float servo_offset = 0.2;
	
	float commandD = 0.f;
	float commandP = 0.f;
	int r = 0;
	
	//      PID        
	
	//STABLE
	float P = 0.011;
	float D = 0.008f;
	float command_engines = 0.4f;
	
	//Stable
	P = 0.0011;
	D = 0.0001f;
	command_engines = 0.53f;
	
	//Stable, mais erreur atteint saturation a vitesse 0.55
	P = 0.011;
	D = 0.0007f;
	command_engines = 0.54f;
	
	//Le plus performant pour l'instant
	P = 0.013;
	D = 0.0007f;
	command_engines = 0.54f;
	
	//To check loop times
	chrono chr_cam_m, chr_loop_m,chr_io,chr_rest, chr_Task;
	//To ensure loop times
	chrono chr_distantio,chr_cam,chr_led,chr_servo;
	float t_cam = 0, t_loop = 0, t_io = 0, t_rest = 0, t_Task = 0;
	float looptime_cam;
	float queue_size = 0;
	
	//TFC_HBRIDGE_ENABLE;
	
	uint16_t pload;
	
	uint8_t led_state = 0;
	TFC_SetBatteryLED_Level(led_state);
	float exposure_time_us = 8000;
	uint32_t servo_update_ms = 20;
	
	//Readonly variables
	register_scalar(&command_engines,FLOAT,1,"command engines");
	register_scalar(&r, INT32,0,"LineState");
	register_scalar(&data.line_position, FLOAT,0,"Error");
	//register_scalar(&data.valid_line_position,FLOAT,0,"Shielded Error");
	//register_scalar(&data.previous_line_position,FLOAT,0,"Correction on/off");
	//register_scalar(&data.hysteresis_threshold,FLOAT,1,"Protection distance");
	//register_scalar(&data.distance,FLOAT,1,"distance variation");
	register_scalar(&error_derivative, FLOAT,0,"Derivative");
	register_scalar(&command,FLOAT,0,"command");
	register_scalar(&commandP,FLOAT,0,"cmdP");
	register_scalar(&commandD,FLOAT,0,"cmdD");
	
	register_scalar(&exposure_time_us,FLOAT,1,"Exposure time");
	
	//Read/write variables
	register_scalar(&P,FLOAT,1,"P");
	register_scalar(&D,FLOAT,1,"D");	
	
	//Calibration data
	register_scalar(&servo_offset,FLOAT,1,"servo_offset");
	register_scalar(&data.linewidth,FLOAT,1,"linewidth");
	register_scalar(&data.offset,FLOAT,1,"offset");
	register_scalar(&data.threshold,INT32,1,"threshold");
	
	register_scalar(&filtered_error,FLOAT,1,"Filtered error");
	register_scalar(&alpha_error,FLOAT,1,"Filtered error Alpha");
	
	//Secondary values
	register_scalar(&pload,UINT16,0,"Serial load");
	
	register_scalar(&t_cam,FLOAT,0,"max cam time(ms)");
	register_scalar(&t_loop,FLOAT,0,"max main time(ms)");
	register_scalar(&t_rest,FLOAT,0,"max rest time(ms)");
	register_scalar(&t_io,FLOAT,0,"distant io time(ms)");
	register_scalar(&t_Task,FLOAT,0,"max TFC task time(ms)");
	register_scalar(&looptime_cam,FLOAT,0,"cam exe period(us)");
	register_scalar(&queue_size, FLOAT,0,"queue size");
	
	register_scalar(&data.edges_count,UINT16,0,"edge count");
	register_array(data.threshold_image,128,INT8,0,"threshold_line");
	register_array(data.derivative_image,128,INT32,0,"line derivative");
	register_array(data.raw_image,128,UINT16,0,"raw_line");
	
	TFC_SetLineScanExposureTime(exposure_time_us);
	TFC_SetServo(0, servo_offset);
	
	Restart(&chr_distantio);
	Restart(&chr_led);
	Restart(&chr_servo);
	Restart(&chr_cam_m);
	Restart(&chr_io);
	Restart(&chr_rest);
	Restart(&chr_Task);
	
	for(;;)
	{
			
		/**TIME MONITORING**/				Restart(&chr_loop_m);					
		
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		/**TIME MONITORING**/				Restart(&chr_Task);
		TFC_Task();
		/**TIME MONITORING**/				Capture(&chr_Task);
		/**TIME MONITORING**/				t_Task = max(t_Task,GetLastDelay_ms(&chr_Task));
		
		/* -------------------------- */
		/* ---- UPDATE DISTANTIO ---- */
		/* -------------------------- */
		Capture(&chr_distantio);
		if(GetLastDelay_us(&chr_distantio) > 3000)
		{
			Restart(&chr_distantio);
			/**TIME MONITORING**/			Restart(&chr_io);
			update_distantio();
			/**TIME MONITORING**/			Capture(&chr_io);
			/**TIME MONITORING**/			t_io = GetLastDelay_ms(&chr_io);
			pload = getPeakLoad();
			/**TIME MONITORING**/			t_rest = 0;
		}
				
		/* -------------------------- */
		/* - UPDATE LINE PROCESSING - */
		/* -------------------------- */
		Capture(&chr_cam);
		looptime_cam = GetLastDelay_us(&chr_cam);
		if(looptime_cam > exposure_time_us)
		{
			Restart(&chr_cam);
			
			/**TIME MONITORING**/			Restart(&chr_cam_m);
			
			r = read_process_data(&data);
			if(r == LINE_OK)
			{
				//compute_valid_line_position(&data,r);
				
				//Compute errors for PD
				previous_error = position_error;
				position_error = data.line_position;
				
				filtered_error = filtered_error * alpha_error +  position_error * (1 - alpha_error);
				
				error_derivative = (position_error - previous_error) * 1000000.f / looptime_cam;
				
				commandP = P * filtered_error;
				commandD = D * error_derivative;							
			}			
			TFC_SetLineScanExposureTime(exposure_time_us);
			
			/**TIME MONITORING**/			Capture(&chr_cam_m); 				
			/**TIME MONITORING**/			t_cam = max(t_cam,GetLastDelay_ms(&chr_cam_m));
		}	
		
		/* -------------------------- */
		/* ---- UPDATE DIRECTION ---- */
		/* -------------------------- */
		Capture(&chr_servo);
		if(GetLastDelay_ms(&chr_servo) > servo_update_ms)
		{
			Restart(&chr_servo);
			
			//Get offset
			servo_offset = TFC_ReadPot(0);
			
			//Compute servo command between -1.0 & 1.0 
			command = commandD + commandP + servo_offset;
			
			if(command > 1.f)
				command = 1.f;
			if(command < -1.f)
				command = -1.f;
							
			TFC_SetServo(0, command);
		}		
		
		/* -------------------------- */
		/* ---   UPDATE ENGINES   --- */
		/* -------------------------- */
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
		
		/* -------------------------- */
		/* --- UPDATE PERIPHERALS --- */
		/* -------------------------- */				
		//Calibration and engine update the rest of the time 
		if(TFC_PUSH_BUTTON_0_PRESSED)
		{
			calibrate_data(&data);
			led_state = 3;
			TFC_SetBatteryLED_Level(led_state);
		}
		
		
		
		/* -------------------------- */
		/* ---  UPDATE ALIVE LED  --- */
		/* -------------------------- */
		Capture(&chr_led);
		if(GetLastDelay_ms(&chr_led) > 500)
		{
			Restart(&chr_led);
			led_state ^= 0x01; 	
			TFC_SetBatteryLED_Level(led_state);
		}
		
		
		
		
		/**TIME MONITORING**/				Restart(&chr_rest);
		/**TIME MONITORING**/				Capture(&chr_loop_m);
		/**TIME MONITORING**/				Capture(&chr_rest);
		/**TIME MONITORING**/				t_loop = max(t_loop,GetLastDelay_ms(&chr_loop_m));
		/**TIME MONITORING**/				t_rest = max(t_rest,GetLastDelay_ms(&chr_rest));
	}
}

void configure_bluetooth()
{
	TFC_Init();
		
		uart0_init (CORE_CLOCK/2/1000, 38400);
		 for(;;) //Try AT Command
		 {   
			 TFC_Task();
			
			 
			 if(TFC_Ticker[0]>1000)
			 {                   
				 
				 
				 serial_printf("AT+UART=115200,0,0\r\n");
				 TFC_Ticker[0] = 0;
				
				 TFC_Task();
				 
				 //Reset uart 
				 //uart0_init (CORE_CLOCK/2/1000, 115200);
			 }
		 }
}

