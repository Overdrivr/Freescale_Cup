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
	uint16_t raw_image[128];
	int32_t derivative_image[128];
	int8_t threshold_image[128];
	uint16_t min, max;
	uint32_t image_integral;
	
	int falling_edges_position[128];
	int rising_edges_position[128];
	uint16_t edges_count;
	float line_position;
	int16_t valid_line_position;
	
	//Calibration data
	int32_t threshold;
	float offset;
	float linewidth;
	
	//Parameters
	int16_t edgeleft;
	int16_t edgeright;	
};

enum
{
	LINE_OK = 0,
	LINE_LOST = -1
};


//////////////////////////////////////////////
void init_data(cameraData* data);
/*
 * Function to init buffers to zero
 * data : data structure for holding all informations (for filtering & computing)
 */



//////////////////////////////////////////////
int read_process_data(cameraData* data);
/*
 * Reads camera image through serial port, process image and computes line position 
 * data : data structure for holding all informations (for filtering, optimizations, etc.)
 * Return : 1 if job done, 0 if done nothing
 */

//////////////////////////////////////////////
void calibrate_data(cameraData* data);
/*
 * Camera will read 10 frames and compute a correct threshold value
 * 
 */


#endif /* CAMERA_PROCESSING_H_ */