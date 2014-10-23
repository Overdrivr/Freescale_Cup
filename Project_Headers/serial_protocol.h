/*
 * serial_protocol.h
 *
 *  Created on: Oct 22, 2014
 *      Author: B48923
 */

#ifndef SERIAL_PROTOCOL_H_
#define SERIAL_PROTOCOL_H_

#include "TFC/TFC.h"
#include "serial.h"

uint8_t SOF_;
uint8_t EOF_;
uint8_t ESC_;


#define INCOMING_FRAME_QUEUE_SIZE 2048

typedef enum state state;
enum state
{
	IDLE,
	IN_PROCESS
};

typedef enum ESC_state ESC_state;
enum ESC_state
{
	NONE,
	NEXT
};

void init_serial_protocol();

/*
 * Serial protocol (see serial_protocols_definition.xlsx for more information)
 */
void send_serial_frame(uint8_t* framedata, uint16_t framesize);


/*
 * Returns framesize if no reception is in process (Either nothing was received or there is a full frame ready for reading)
 * 		   0 otherwise
 */
uint16_t full_serial_frame_available();


/*
 * Returns first byte of received frame
 * If no frame is available, returns 0x00 - call first serial_frame_available to ensure state
 */
uint8_t read_serial_frame();


/*
 * Processes raw serial reception queue
 * Call as often as possible
 */
void update_serial_protocol();

#endif /* SERIAL_PROTOCOL_H_ */
