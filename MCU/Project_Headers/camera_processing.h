/*
 * camera_processing.h
 *
 *  Created on: Aug 19, 2014
 *      Author: B48923
 */

#ifndef CAMERA_PROCESSING_H_
#define CAMERA_PROCESSING_H_

#include "derivative.h"
#include "TFC\TFC.h"

typedef struct cameraData cameraData;
struct cameraData
{
	
	int32_t derivative_zero[128];
	int32_t derivative_image[128];
	uint16_t raw_image[128];
	int8_t threshold_image[128];
	
	int falling_edges_position[128];
	int rising_edges_position[128];
	float line_position;
	float previous_line_position;
	float filter_coeff;
	int32_t linestate;
	int32_t current_linewidth;
	int32_t current_linewidth_diff;
	
	int32_t threshold;
	int32_t linewidth;
	int32_t linewidth_margin;
	int32_t linewidth1, linewidth2;
	
	float offset;
	float halftrack_width;
	uint16_t edges_count;
	
	int16_t edgeleft;
	int16_t edgeright;	
	
};

enum
{
	LINE_OK = 0,
	LINE_LOST = -1,
	LINE_UNSURE = -2
};


//////////////////////////////////////////////
void init_data(cameraData* data);
/*
 * Function to init buffers to zero
 * data : data structure for holding all informations (for filtering & computing)
 */

//////////////////////////////////////////////
void read_data(cameraData* data);
/*
 * Function to read the camera with the ADC
 * data : data structure for holding all informations (for filtering & computing)
 */

//////////////////////////////////////////////
int process_data(cameraData* data);
/*
 * Reads camera image through serial port, process image and computes line position 
 * data : data structure for holding all informations (for filtering, optimizations, etc.)
 * Return : 1 if job done, 0 if done nothing
 */

//////////////////////////////////////////////
void calibrate_data(cameraData* data, uint32_t exposure_time_us);
/*
 * Camera reads a zero on the derivative
 * 
 */

#endif /* CAMERA_PROCESSING_H_ */
