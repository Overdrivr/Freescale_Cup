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
	uint16_t filtered_image[128];
	int8_t threshold_image[128];
	uint16_t falling_edges_position[128];
	uint16_t rising_edges_position[128];
	uint16_t edges_count;
	int16_t line_position;
};


//////////////////////////////////////////////
void initData(cameraData* data);
/*
Function to init buffers to zero
 
 * data : data structure for holding all informations (for filtering, optimizations, etc.)
 */



//////////////////////////////////////////////
int readNProcessData(cameraData* data, float alpha);
/*
Reads camera image through serial port, process image and computes line position 
 * data : data structure for holding all informations (for filtering, optimizations, etc.)
 * alpha : Coefficient for complementary filter, between ]0.0;1.0[
 	 	   Value close to 1.0 filters a lot, but reacts slowly to new changes
 	 	   	   	    	  0.0 filters very little, the new image replaces almost immediately the old one, so does noise
 */


#endif /* CAMERA_PROCESSING_H_ */
