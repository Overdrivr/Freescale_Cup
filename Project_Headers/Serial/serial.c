/*
 * serial.c
 *
 *  Created on: Oct 13, 2014
 *      Author: B48923
 */

// Copyright (C) 2014 Rémi Bèges
// For conditions of distribution and use, see copyright notice in the LICENSE file

#include "serial.h"
#define SERIAL_NO_OVERWRITE

uint16_t peak_load;

void init_serial()
{
	peak_load = 0;
}

uint16_t getPeakLoad()
{
	return peak_load; 
}

int16_t serial_write(uint8_t* buf, uint16_t len)
{
	//To monitor peak load
	static uint16_t load = 0;
	
	load = BytesInQueue(&SERIAL_OUTGOING_QUEUE);
	
	if(load > peak_load)
	{
		peak_load = load;
	}
	
	#if defined(SERIAL_NO_OVERWRITE)
		while(load + len > SERIAL_OUTGOING_QUEUE.QueueSize)
		{
			load = BytesInQueue(&SERIAL_OUTGOING_QUEUE);
		}
	#endif
	
	//Disable Transmitter Interrupts
	UART0_C2 &= ~UART_C2_TIE_MASK; 
	
	ByteArrayEnqueue(&SERIAL_OUTGOING_QUEUE,buf,len);
	
	//Re-enable Transmitter Interrupts if needed
	if(BytesInQueue(&SDA_SERIAL_OUTGOING_QUEUE)>0 && (UART0_S1 & UART_S1_TDRE_MASK))
		UART0_C2 |= UART_C2_TIE_MASK; 
	
	return QUEUE_OK;
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
