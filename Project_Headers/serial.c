/*
 * serial.c
 *
 *  Created on: Oct 13, 2014
 *      Author: B48923
 */


#include "serial.h"
#include "TFC/TFC_UART.h"
#include "TFC/TFC_Queue.h"


int16_t serial_write(uint8_t* buf, uint16_t len)
{
	return ByteArrayEnqueue(&SERIAL_OUTGOING_QUEUE,buf,len);
}

uint16_t serial_available()
{
	return BytesInQueue(&SERIAL_INCOMING_QUEUE);
}

uint8_t serial_read()
{
	return ForcedByteDequeue(&SERIAL_INCOMING_QUEUE);
}
