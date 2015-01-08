/*
 * serial.c
 *
 *  Created on: Oct 13, 2014
 *      Author: B48923
 */

// Copyright (C) 2014 Rémi Bèges
// For conditions of distribution and use, see copyright notice in the LICENSE file

#include "serial.h"

int16_t serial_write(uint8_t* buf, uint16_t len)
{
	return ByteArrayEnqueue(&SERIAL_OUTGOING_QUEUE,buf,len);
}

uint16_t serial_write_available()
{
	return (SERIAL_OUTGOING_QUEUE.QueueSize - BytesInQueue(&SERIAL_OUTGOING_QUEUE));
}

uint16_t serial_available()
{
	return BytesInQueue(&SERIAL_INCOMING_QUEUE);
}

uint8_t serial_read()
{
	return ForcedByteDequeue(&SERIAL_INCOMING_QUEUE);
}
