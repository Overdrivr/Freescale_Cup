/*
 * serial_protocol.c
 *
 *  Created on: Oct 22, 2014
 *      Author: B48923
 */

#include "protocol.h"
#include "distantio.h"

ByteQueue rx_frame;
uint8_t rx_frame_storage[INCOMING_FRAME_QUEUE_SIZE];

//Protocol state:
state protocol_state;

//In case ESC character is received
ESC_state escape_state;

uint8_t SOF_;
uint8_t EOF_;
uint8_t ESC_;


void init_protocol()
{
	InitByteQueue(&rx_frame,INCOMING_FRAME_QUEUE_SIZE,rx_frame_storage);
	protocol_state = IDLE;
	escape_state = NONE;
	
	SOF_ = 0xF7;
	EOF_ = 0x7F;
	ESC_ = 0x7D;
}

void protocol_frame(uint8_t* framedata, uint16_t framesize)
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



void protocol_frame_begin()
{	
	//Write start of frame byte
	serial_write(&SOF_,1);
}

void protocol_frame_append(uint8_t* framedata,uint16_t framesize)
{
	uint16_t i;
	//Write data
	for(i = 0 ; i < framesize ; i++)
	{
		//See serial_protocols_definition.xlsx
		if(framedata[i] == SOF_ ||
		   framedata[i] == EOF_ ||
		   framedata[i] == ESC_)
		{
			//If data contains one of the flags, we escape it before
			serial_write(&ESC_,1);
		}
		serial_write(framedata + i,1);
	}
}

void protocol_frame_end()
{	
	//Set EOFrame
	serial_write(&EOF_,1);	
}


void protocol_process_rx()
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
					distantio_decode_rx_frame(&rx_frame);
					
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


