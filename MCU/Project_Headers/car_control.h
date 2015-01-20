/*
 * car_control.h
 *
 *  Created on: Jan 19, 2015
 *      Author: B48923
 */

#ifndef CAR_CONTROL_H_
#define CAR_CONTROL_H_

#include "TFC/TFC.h"

typedef struct carData carData;
struct carData
{
	//Parameters
	float P;
	float D;
	float servo_offset;
	float alpha_filtered_error;
	
	//Time-varying controls
	float command_engines;
	float commandD;
	float commandP;
	
	//Time varying values
	float previous_error;
	float error_derivative;
	
	float filtered_error;
	float command;	
};

//To get the singleton pointer
carData* carcontrol_getdatahandle();

//To call AFTER line has been read and processed
void carcontrol_line_update(float position_error, int8_t linestate);

//To call WHEN finish line was detected
void carcontrol_finish_line();

//To call BEFORE servo update
void carcontrol_preprocess_servo();

//To call BEFORE motor speed update
void carcontrol_preprocess_engines();

#endif /* CAR_CONTROL_H_ */
