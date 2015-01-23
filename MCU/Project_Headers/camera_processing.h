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
	int32_t derivative_zero[128];
	int32_t derivative_image[128];
	int8_t threshold_image[128];
	uint16_t min, max;
	uint32_t image_integral;
	uint32_t reference_integral;
	
	int falling_edges_position[128];
	int rising_edges_position[128];
	uint16_t edges_count;
	float line_position;
	float previous_line_position;
	float valid_line_position;
	float distance;
	float error;
	int32_t linestate;
	float current_linewidth;
	float current_linewidth_diff;
	
	//Calibration data
	int32_t threshold;
	float offset;
	float linewidth;
	float halftrack_width;
	uint16_t deglitch_counter;
	uint16_t deglitch_limit;
	
	//Parameters
	int16_t edgeleft;
	int16_t edgeright;	
	float hysteresis_threshold;
};

enum
{
	LINE_OK = 0,
	LINE_LOST = -1,
	LINE_UNSURE = -2
};

enum
{
	LINE_TRACK_LEFT = -5,//Only left track border is visible
	LINE_HALF_TRACK_LEFT = -4,//Half of left track border is visible
	LINE_NOTHING_LEFT = -3,//Neither line nor track border is visible
	LINE_HALF_LEFT = -2,//left side of the line is visible
	LINE_LEFT = -1,
	LINE_CENTER = 0,
	LINE_RIGHT = 1,
	LINE_HALF_RIGHT = 2,
	LINE_NOTHING_RIGHT = 3,
	LINE_HALF_TRACK_RIGHT = 4,
	LINE_TRACK_RIGHT = 5
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

void compute_valid_line_position(cameraData* data, int linestate);


#endif /* CAMERA_PROCESSING_H_ */
