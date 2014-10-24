/*
 * serial_protocol.c
 *
 *  Created on: Oct 22, 2014
 *      Author: B48923
 */

#include "serial_protocol.h"


ByteQueue rx_frame;
uint8_t rx_frame_storage[INCOMING_FRAME_QUEUE_SIZE];

//Protocol state:
state protocol_state;

//In case ESC character is received
ESC_state escape_state;

void init_serial_protocol()
{
	InitByteQueue(&rx_frame,INCOMING_FRAME_QUEUE_SIZE,rx_frame_storage);
	protocol_state = IDLE;
	escape_state = IDLE;
	
	SOF_ = 0x7F;
	EOF_ = 0x7F;
	ESC_ = 0x7D;
}

void send_serial_frame(uint8_t* framedata, uint16_t framesize)
{
	uint16_t i;
	
	//Write start of frame byte
	serial_write(&SOF_,1);
	
	//Write data
	for(i = 0 ; i < framesize ; i++)
	{
		//See serial_protocols_definition.xlsx
		if(*(framedata + i) == SOF_ ||
		   *(framedata + i) == EOF_ ||
		   *(framedata + i) == ESC_)
		{
			//If data contains one of the flags, we escape it before
			serial_write(&ESC_,1);
		}
		serial_write(framedata + i,1);
	}
	
	//Set EOFrame
	serial_write(&EOF_,1);	
}

uint16_t full_serial_frame_available()
{
	if(protocol_state == 0)
		return BytesInQueue(&rx_frame);
	else
		return 0;
}

uint8_t get_serial_frame()
{
	return ForcedByteDequeue(&rx_frame);
}

void update_serial_protocol()
{
	uint8_t received_byte;
	
	//Process received bytes
	while(serial_available())
	{
		received_byte = serial_read();
		
		//If a reception was in process
		if(protocol_state == IN_PROCESS)
		{
			//If the character must be ignored as SOF, EOF or ESC
			if(escape_state == NEXT)
			{
				ByteEnqueue(&rx_frame,received_byte);
				escape_state = NONE;
			}
			else
			{
				if(received_byte == EOF_)
				{
					protocol_state = IDLE;
				}
				else if(received_byte == ESC_)
				{
					escape_state = NEXT;
				}
				else
				{
					ByteEnqueue(&rx_frame,received_byte);
				}
			}
		}
		else
		{
			if(received_byte == SOF_)
			{
				protocol_state = IN_PROCESS;
			}
			else
			{
				//Ignore character
				//Could dump into secondary queue
			}
		}
		
	}
}