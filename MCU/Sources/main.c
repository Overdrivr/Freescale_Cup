#include "derivative.h" /* include peripheral declarations */
#include "TFC/TFC.h"
#include "camera_processing.h"
#include "DistantIO/distantio.h"
#include "chrono.h"
#include "UnitTests/UnitTests.h"
#include "TFC/TFC_UART.h"

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
	//test_alive_signal();
	
	cam_program();
	
	//configure_bluetooth();
	
	return 0;
}

void cam_program()
{
	/* --- INIT --- */
	TFC_Init();
	init_serial();
	init_protocol();
	init_distantio(3);
	cameraData data;
	init_data(&data);
	
	/* --- COMPUTATION VARIABLES --- */
	float previous_error = 0.f, error_proportionnal = 0.f, error_derivative = 0.f;
	float command = 0.f, commandD = 0.f, commandP = 0.f;
	int linestate;
	int32_t current_gear = 0;
	
	chrono chr_distantio,chr_cam,chr_led,chr_servo, chr_engines;
	float looptime_cam;
	float looptime_servo;
	float looptime_engines;
	
	uint32_t counter_fps = 0,fps = 0;
	chrono chrono_fps;
	
	uint32_t counter_process_fps = 0, process_fps = 0;
	chrono chrono_process_fps;
	
	uint32_t counter_direction_fps,direction_fps;
	chrono chrono_direction_fps;
	
	uint32_t counter_engines_fps,engines_fps;
	chrono chrono_engines_fps;
	
	uint8_t led_state = 0;
	
	float servo_speed = 0.f;
	//float previous_cmd = 0.f;
	
	float commandleft = 0.f;
	float commandright = 0.f;
	
	float ecart = 0.f;
	
	//uint32_t acceleration_counter = 0;
	//uint32_t breaking_counter;
	//uint8_t turn_state = 0;//0 when on straight line, 1 on a turn
	float direction_CMA = 0.f;
	float direction_old_CMA = 0.f;
	uint8_t flag_straight_line; // 0 if on a turn, 1 on a straight line
	
	
	/* --- PARAMETERS --- */
	//	PID        
	float P[3];
	float D[3];
	P[0] = 0.013f;//0.015f
	D[0] = 0.00095f;//0.0012f
	P[1] = 0.013f;//0.015f
	D[1] = 0.0007f;//0.0009
	P[2] = 0.007f;
	D[2] = 0.f;
		
	// Direction
	float servo_offset = 0.2;
	uint32_t CMA_amount = 25;
	float CMA_level = 0.3;
	
	// Engines, gearbox & differential drive
	int32_t killswitch = 0; // Engines initially off
	int32_t gears_bypass = 0; // gearbox initially on
	
	float gear_1_threshold = 5;
	float gears[3];
	/*gears[0] = 0.55f; // Line lost speed
	gears[1] = 0.64f; // Error > gear_1_threshold
	gears[2] = 0.78f;*/ // Full speed
	gears[0] = 0.5f; // Line lost speed
	gears[1] = 0.55f; // Error > gear_1_threshold
	gears[2] = 0.6f;
	
	float coeff_pos = 0.6f;
	float coeff_neg = 0.6f;
	float offset_in = 0.5;
	float command_threshold = 0.4;
	
	uint32_t acceleration_width = 10;
	uint32_t breaking_width = 10;
	
	// Refresh rates control
	
	float exposure_time_us = 6000;
	float servo_update_us = 10000;
	float engines_update_us = 5000;
	
	/* --- DISTANTIO REGISTRATIONS --- */
	
	// Start - Stop
	register_scalar(&killswitch,INT32,1,"Killswitch. 0 = car stop");
	
	// Gearbox
	register_scalar(&gears[0],FLOAT,1,"gears[0]");
	register_scalar(&gears[1],FLOAT,1,"gears[1]");
	register_scalar(&gears[2],FLOAT,1,"gears[2]");
	register_scalar(&gears_bypass,INT32,1,"automatic gear bypass (if != 0)");
	register_scalar(&current_gear,INT32,1,"current gear");
	register_scalar(&gear_1_threshold,FLOAT,1,"gear 1<->2 threshold");
	
	// Differential drive & Engines
	register_scalar(&commandleft,FLOAT,0,"Engine left");
	register_scalar(&commandright,FLOAT,0,"Engine right");
	register_scalar(&ecart,FLOAT,0,"Engine diff action");
	register_scalar(&coeff_pos,FLOAT,1,"diff coeff pos");
	register_scalar(&coeff_neg,FLOAT,1,"diff coeff min");
	register_scalar(&offset_in,FLOAT,1,"offset in");
	register_scalar(&command_threshold,FLOAT,1,"command threshold");
	
	// Direction
	register_scalar(&error_proportionnal, FLOAT,0,"Proportionnel");
	register_scalar(&error_derivative, FLOAT,0,"Derivative");
	register_scalar(&command,FLOAT,0,"CMD PD");
	register_scalar(&commandP,FLOAT,0,"CMD P");
	register_scalar(&commandD,FLOAT,0,"CMD D");
	register_scalar(&servo_speed,FLOAT,0,"servo_speed");
	register_scalar(&P[0],FLOAT,1,"P0");
	register_scalar(&D[0],FLOAT,1,"D0");
	register_scalar(&P[1],FLOAT,1,"P1");
	register_scalar(&D[1],FLOAT,1,"D1");
	register_scalar(&P[2],FLOAT,1,"P2");
	register_scalar(&D[2],FLOAT,1,"D2");
	register_scalar(&CMA_amount,UINT32,1,"direction CMA amount");
	register_scalar(&direction_CMA,FLOAT,0,"direction CMA");
	register_scalar(&CMA_level,FLOAT,1,"CMA level trigger");
	register_scalar(&flag_straight_line,UINT8,0,"CMA flag (0 = turn)");
		
	// Camera
	register_scalar(&data.current_linewidth, INT32,0,"Linewidth current");
	register_scalar(&data.current_linewidth_diff, INT32,0,"Linewidth error");
	register_scalar(&data.linewidth_margin, INT32,1,"Linewidth margin");
	register_scalar(&data.linewidth1, INT32,0,"Linewidth 1");
	register_scalar(&data.linewidth2, INT32,0,"Linewidth 2");
	register_scalar(&data.edges_count,UINT16,0,"edge count");
	
	// Refresh
	register_scalar(&exposure_time_us,FLOAT,1,"Camera refresh (us)");
	register_scalar(&servo_update_us,FLOAT,1,"Direction refresh (us)");
	register_scalar(&engines_update_us,FLOAT,1,"Engines refresh (us)");
	
	// FPS
	register_scalar(&fps,UINT32,0,"FPS");
	register_scalar(&process_fps,UINT32,0,"camera FPS");
	register_scalar(&direction_fps,UINT32,0,"direction FPS");
	register_scalar(&engines_fps,UINT32,0,"direction FPS");
	
	//Calibration data
	register_scalar(&servo_offset,FLOAT,1,"servo_offset");
	register_scalar(&data.offset,FLOAT,1,"line offset");
	register_scalar(&data.threshold,INT32,1,"line threshold");
	
	//Secondary values
	//register_scalar(&pload,UINT16,0,"Serial load");
	//register_scalar(&queue_size, FLOAT,0,"queue size");
	
	register_array(data.threshold_image,128,INT8,0,"threshold_line");
	register_array(data.derivative_image,128,INT32,0,"line derivative");
	register_array(data.derivative_zero,128,INT32,0,"line derivative zero");
	register_array(data.raw_image,128,UINT16,0,"raw_line");
	
	/* --- SECONDARY INITS --- */
	TFC_SetBatteryLED_Level(led_state);
	TFC_SetServo(0, servo_offset);
	reset(&chr_distantio);
	reset(&chr_led);
	reset(&chr_servo);
	reset(&chrono_fps);
	reset(&chrono_process_fps);
	//acceleration_counter = acceleration_width;
	//breaking_counter = breaking_width;
	
	for(;;)
	{		
		TFC_Task();
		
		/* -------------------------- */
		/* ---- UPDATE DISTANTIO ---- */
		/* -------------------------- */
		if(us(&chr_distantio) > 100000.f)
		{
			reset(&chr_distantio);
			
			update_distantio();
		}
		
		//Compute fps
		if(ms(&chrono_fps) < 1000)
			counter_fps++;
		else
		{
			fps = counter_fps;
			counter_fps = 0;
			reset(&chrono_fps);
		}
				
		/* -------------------------- */
		/* - UPDATE LINE PROCESSING - */
		/* -------------------------- */
		looptime_cam = us(&chr_cam);
		if(looptime_cam > exposure_time_us)
		{
			reset(&chr_cam);
			
			read_data(&data);
			
			//Compute fps
			if(ms(&chrono_process_fps) < 1000)
				counter_process_fps++;
			else
			{
				process_fps = counter_process_fps;
				counter_process_fps = 0;
				reset(&chrono_process_fps);
			}
			
			linestate = process_data(&data);
			if(linestate == LINE_OK)
			{				
				//Compute errors for PD
				previous_error = error_proportionnal;
				error_proportionnal = data.line_position;
				error_derivative = (error_proportionnal - previous_error) * 1000000.f / looptime_cam;
				
				//Gear bypass disconnect autmatic gear switching
				if(gears_bypass == 0)
				{
					//Adjust gear depending on error
					if(error_proportionnal > gear_1_threshold || error_proportionnal < -gear_1_threshold)
					{
						current_gear = 1;
					}
					else
						current_gear = 2;
				}
					
				
			}
			else
			{
				error_derivative = 0;
				
				if(gears_bypass == 0)
					current_gear = 0;
			}
		}	
		
		/* -------------------------- */
		/* ---- UPDATE DIRECTION ---- */
		/* -------------------------- */
		looptime_servo = us(&chr_servo);
		if(looptime_servo > servo_update_us)
		{			
			reset(&chr_servo);
			
			
			//Compute fps
			//TO TEST
			if(ms(&chrono_direction_fps) < 1000)
				counter_direction_fps++;
			else
			{
				direction_fps = counter_direction_fps;
				counter_direction_fps = 0;
				//remove_ms(&chrono_direction_fps,1000);
				reset(&chrono_direction_fps);
			}
						
			
			commandP = P[current_gear] * error_proportionnal;
			commandD = D[current_gear] * error_derivative;
			
			//Get offset
			//servo_offset = TFC_ReadPot(0);
			
			//previous_cmd = command;
			
			//Compute servo command between -1.0 & 1.0 
			command = commandD + commandP + servo_offset;
			
			if(command > 1.f)
				command = 1.f;
			if(command < -1.f)
				command = -1.f;
							
			TFC_SetServo(0, command);
			
			//Compute servo speed
			//servo_speed = (command - previous_cmd) / (looptime_servo * 1000.f);
			
			// Compute direction rolling average
			direction_old_CMA = direction_CMA;
			direction_CMA = (command + CMA_amount * direction_old_CMA) / (CMA_amount + 1);
			
			// Determine if the car is on a straight line or turning
			if(direction_CMA > CMA_level || direction_CMA < -CMA_level)
				flag_straight_line = 0;
			else
				flag_straight_line = 1;
		}		
		
		/* -------------------------- */
		/* ---   UPDATE ENGINES   --- */
		/* -------------------------- */
		looptime_engines= us(&chr_engines);
		if(looptime_engines > engines_update_us)
		{
			reset(&chr_engines);
		
			//Compute fps
			if(ms(&chrono_engines_fps) < 1000)
				counter_engines_fps++;
			else
			{
				engines_fps = counter_engines_fps;
				counter_engines_fps = 0;
				reset(&chrono_engines_fps);
			}
			
			if(killswitch == 0)
			{
				TFC_HBRIDGE_DISABLE;
				TFC_SetMotorPWM(0 , 0);
			}
			else
			{
				TFC_HBRIDGE_ENABLE;
				if(current_gear > 2)
					current_gear = 0;
				
				if(command < 0)
				{
					ecart = -command/2.f;
					commandleft = gears[current_gear] * (1 - ecart*coeff_neg);//OK
					commandright = gears[current_gear]* (1 + ecart*coeff_pos);
				}
				else if(command > 0)
				{
					ecart = command/2.f;
					commandleft = gears[current_gear] * (1 + ecart*coeff_pos);
					commandright = gears[current_gear]* (1 - ecart*coeff_neg);//OK
				}
				else
				{
					commandleft = gears[current_gear];
					commandright = gears[current_gear];
				}
				
				//Brake on beginning of turn
				
				//Accelerate on end of turn
				
				
				//Brake with inner wheel in case of strong turn ?
				/*if(command < -command_threshold)
					commandleft -= offset_in;
				else if(command > command_threshold)
					commandright -= offset_in;
				*/
				TFC_SetMotorPWM(commandleft, commandright);
			}
		}
		
		/* -------------------------- */
		/* --- UPDATE PERIPHERALS --- */
		/* -------------------------- */				
		//Calibration and engine update the rest of the time 
		if(TFC_PUSH_BUTTON_0_PRESSED)
		{
			calibrate_data(&data,exposure_time_us);
			led_state = 3;
			TFC_SetBatteryLED_Level(led_state);
		}
		
		/* -------------------------- */
		/* ---  UPDATE ALIVE LED  --- */
		/* -------------------------- */
		if(ms(&chr_led) > 500)
		{
			reset(&chr_led);
			led_state ^= 0x01; 	
			TFC_SetBatteryLED_Level(led_state);
		}
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

