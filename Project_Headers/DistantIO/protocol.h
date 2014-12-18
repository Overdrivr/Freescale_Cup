/*
 * serial_protocol.h
 *
 *  Created on: Oct 22, 2014
 *      Author: B48923
 */

#ifndef SERIAL_PROTOCOL_H_
#define SERIAL_PROTOCOL_H_

#include "TFC/TFC.h"
#include "..\Serial\serial.h"

uint8_t SOF_;
uint8_t EOF_;
uint8_t ESC_;


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

void init_serial_protocol();

/*
 * Serial protocol (see serial_protocols_definition.xlsx for more information)
 */
void send_serial_frame(uint8_t* framedata, uint16_t framesize);

/*
 * Alternative send_serial_frame to send data in chunks
 */
void start_serial_frame();
void append_serial_frame(uint8_t* framedata,uint16_t framesize);
void end_serial_frame();
/*
 * Processes raw serial reception queue
 * Call as often as possible
 */
void update_serial_protocol();

#endif /* SERIAL_PROTOCOL_H_ */
