/*
 * serial_protocol.h
 *
 *  Created on: Oct 22, 2014
 *      Author: B48923
 */

#ifndef SERIAL_PROTOCOL_H_
#define SERIAL_PROTOCOL_H_

#include "serial.h"
#include "TFC/TFC.h"

void send_serial_frame(uint8_t* framedata, uint16_t framesize);

/*
 * Returns 1 if frame is available
 */
uint8_t serial_frame_available();

/*
 * Returns 0 if no received frame is available
 * 		   framesize otherwise
 */
uint16_t get_serial_frame(uint8_t* framedata);


#endif /* SERIAL_PROTOCOL_H_ */
