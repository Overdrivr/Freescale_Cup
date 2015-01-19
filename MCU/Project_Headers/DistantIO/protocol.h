/*
 * serial_protocol.h
 *
 *  Created on: Oct 22, 2014
 *      Author: B48923
 */

#ifndef SERIAL_PROTOCOL_H_
#define SERIAL_PROTOCOL_H_

#include "TFC/TFC.h"
#include "Serial/serial.h"

#define INCOMING_FRAME_QUEUE_SIZE 2048

enum state
{
	IDLE,
	IN_PROCESS
};
typedef enum state state;


enum ESC_state
{
	NONE,
	NEXT
};
typedef enum ESC_state ESC_state;

void init_protocol();

/*
 * Serial protocol (see serial_protocols_definition.xlsx for more information)
 */
void protocol_frame(uint8_t* framedata, uint16_t framesize);

/*
 * Alternative send_serial_frame to send data in chunks
 */
void protocol_frame_begin();
void protocol_frame_append(uint8_t* framedata,uint16_t framesize);
void protocol_frame_end();
/*
 * Called by DistantIO
 */
void protocol_process_rx();

#endif /* SERIAL_PROTOCOL_H_ */
