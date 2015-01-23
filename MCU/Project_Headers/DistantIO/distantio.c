/*
 * logger.c
 *
 *  Created on: Oct 13, 2014
 *      Author: B48923
 */

#include "distantio.h"
#include "./serial.h"
#include "chrono.h"

uint8_t register_(uint8_t* adress, uint16_t octets, datatype type, uint8_t writeable, char* name);
void distantio_decode_rx_frame(ByteQueue* rx_queue);
void distantio_send_table();
//Returns 0xFF00 if done, or 0x00XX if transmitted only up to index XX
//TODO : IMPLEMENT DISTRIBUTED TRANSMIT
uint32_t distantio_send_variable(uint16_t variable_id);

log Log;
uint32_t tmp;
chrono chr;

void init_distantio()
{
	uint16_t i;
	char default_name[] = {"undefined"};
	Log.current_index = 0;
	Log.previous_index = 0;
	for(i = 0 ; i < 128 ; i++)
	{
		Log.variables[i].size = 0;
		Log.variables[i].ptr = 0;
		Log.variables[i].writeable = 0;
		Log.variables[i].id = i;
		strcpy(Log.variables[i].name,default_name);
		Log.variables[i].send = 0;
	}
	tmp=0;
	Restart(&chr);
}

//TODO : Replace octets by array_size ?
//Returns 0 if ok, 1 if size exceeds log capacity
uint8_t register_(uint8_t* adress, uint16_t octets, datatype type, uint8_t writeable, char* name)
{
	if(Log.current_index == 127)
		return 1;
	
	//Limited to 1024 octets per variable, should be enough for now
	if(octets > 1024)
		return 1;
	
	Log.variables[Log.current_index].ptr = adress;
	Log.variables[Log.current_index].size = octets;
	Log.variables[Log.current_index].writeable = writeable;
	Log.variables[Log.current_index].type = type;
	strcpy(Log.variables[Log.current_index].name,name);
	
	Log.current_index++;
	
	return 0;
}

uint8_t register_scalar(void* adress, datatype type, uint8_t writeable, char* name)
{
	uint16_t octets = 0;
	switch(type)
	{
	case FLOAT:
		octets = 4;
		break;
		
	case UINT8:
		octets = 1;
		break;
	
	case UINT16:
		octets = 2;
		break;
	
	case UINT32:
		octets = 4;
		break;
		
	case INT8:
		octets = 1;
		break;
	
	case INT16:
		octets = 2;
		break;
		
	case INT32:
		octets = 4;
		break;
	
	default:
		octets = 1;
		break;
	}
	
	return register_((uint8_t*)(adress), octets, type, writeable, name);
}

uint8_t register_array(void* adress, uint16_t size, datatype type, uint8_t writeable, char* name)
{
	uint16_t octets = 0;
		switch(type)
		{
		case FLOAT:
			octets = 4;
			break;
			
		case UINT8:
			octets = 1;
			break;
		
		case UINT16:
			octets = 2;
			break;
		
		case UINT32:
			octets = 4;
			break;
			
		case INT8:
			octets = 1;
			break;
		
		case INT16:
			octets = 2;
			break;
			
		case INT32:
			octets = 4;
			break;
		
		default:
			octets = 1;
			break;
		}
		
	return register_((uint8_t*)(adress), octets * size, type, writeable, name);
}

void update_distantio()
{
	uint16_t i;	
	uint16_t interval = 1;
	
	//Transmit data to serial - Only one variable per mainloop
	for(i = Log.previous_index ; i < Log.previous_index + interval ; i++)
	{		
		if(Log.variables[i].send == 0)
			continue;
		
		distantio_send_variable(i);
	}
	
	
	Log.previous_index += interval;
	if(Log.previous_index >= Log.current_index)
	{
		Log.previous_index = 0;
	}
	
	//Finally, let protocol process rx queue
	protocol_process_rx();
}


void distantio_decode_rx_frame(ByteQueue* rx_queue)
{
	//Get payload byte by byte
	
	uint8_t byte = ForcedByteDequeue(rx_queue);
	uint8_t byte2;
	uint16_t id;
	
	void* ptr;
	
	float* tmp_float;
	uint32_t* tmp_uint32;
	
	float* to_float;
	int32_t* to_int;
	
	uint8_t bytes[4]; 
	
	//If command is return variable
	if(byte == 0x00 && BytesInQueue(rx_queue) >= 2)
	{
		byte = ForcedByteDequeue(rx_queue);
		byte2 = ForcedByteDequeue(rx_queue);
		
		id = byte + (byte2<<8);
		
		if(id < Log.current_index)
		{
			Log.variables[id].send = 1;
		}
	}
	//If command is write variable value
	else if(byte == 0x01)
	{
		byte = ForcedByteDequeue(rx_queue);
		byte2 = ForcedByteDequeue(rx_queue);
		
		id = byte;
		//TOCHECK : 4 or 8 shift ?
		id = byte + (byte2<<8);
		
		if(id < Log.current_index)
		{
			if(Log.variables[id].writeable == 1 && BytesInQueue(rx_queue) >= 4)
			{
				bytes[0] = ForcedByteDequeue(rx_queue);
				bytes[1] = ForcedByteDequeue(rx_queue);
				bytes[2] = ForcedByteDequeue(rx_queue);
				bytes[3] = ForcedByteDequeue(rx_queue);
									
				if(Log.variables[id].type == 0x00)
				{					
					to_float = (float *)(&bytes[0]);
										
					ptr = (void *)(Log.variables[id].ptr);
					tmp_float = (float *)(ptr);
					
					*tmp_float = *to_float;
				}
				else if(Log.variables[id].type == 0x06)
				{
					//TODO : Use void ptr
					to_int = (int32_t*)(&bytes[0]);
					
					ptr = (void *)(Log.variables[id].ptr);
					tmp_uint32 = (uint32_t*)(ptr);
							
					*tmp_uint32 = *to_int;
				}
			}
			else
			{
				
			}
		}
	}
	//If command is return table to master
	else if(byte == 0x02)
	{
		distantio_send_table();
	}
	//If command is stop sending variable
	else if(byte == 0x03 && BytesInQueue(rx_queue) >= 2)
	{
		byte = ForcedByteDequeue(rx_queue);
		byte2 = ForcedByteDequeue(rx_queue);
		
		id = byte + (byte2<<8);
		
		if(id < Log.current_index)
		{
			Log.variables[id].send = 0;
		}
	}
	//If command is stop sending all variables
	else if(byte == 0x04)
	{
		for(id = 0 ; id < Log.current_index ; id++)
		{
			Log.variables[id].send = 0;
		}
	}
	
	//Clean queue
	while(BytesInQueue(rx_queue))
		ForcedByteDequeue(rx_queue);
}


void distantio_send_table()
{
	uint32_t i,j;
	uint8_t *temp_ptr;
	uint8_t type;
	
	uint8_t buffer[512] = {0};
	j = 0;
	
	//Return table command
	buffer[j] = 0x02;			j++;
	
	//Start frame
	protocol_frame_begin();
	protocol_frame_append(buffer,j);
	j = 0;
	
	//variables info
	for(i = 0 ; i < Log.current_index ; i++)
	{		
		//Write data type
		switch(Log.variables[i].type)
		{
			case FLOAT:
				type = 0x00;
				break;
			case UINT8:
				type = 0x01;
				break;
			
			case UINT16:
				type = 0x02;
				break;
			
			case UINT32:
				type = 0x03;
				break;
				
			case INT8:
				type = 0x04;
				break;
			
			case INT16:
				type = 0x05;
				break;
				
			case INT32:
			default:
				type = 0x06;
				break;
		}
		
		if(Log.variables[i].writeable == 1)
			type += 0xF0;
		
		buffer[j] = type;			j++;
		
		//Data id
		//TODO : USE void* instead ???
		temp_ptr = (uint8_t*)(&i);
		buffer[j] = *temp_ptr;		j++;
		buffer[j] = *(temp_ptr+1);	j++;
		
		temp_ptr = (uint8_t*)(&(Log.variables[i].size));
		buffer[j] = *temp_ptr;		j++;
		buffer[j] = *(temp_ptr+1);	j++;				
		
		//Write name
		uint32_t k = 0;
		
		for(k = 0 ; k < 32 ; k++)
		{
			if(k < strlen(Log.variables[i].name))
				buffer[j] = Log.variables[i].name[k];
			else
				buffer[j] = 0;
			j++;
		}
		protocol_frame_append(buffer,j);
		j = 0;
	
	}
	protocol_frame_end();
}

uint32_t distantio_send_variable(uint16_t i)
{
	uint16_t j,k;
	uint8_t *temp_ptr;
	uint8_t buffer[512];
	j = 0;
	
	//Write command
	buffer[j] = 0x00;			j++;
	
	//Write variable ID
	temp_ptr = (uint8_t*)(&i);
	
	buffer[j] = *temp_ptr;		j++;
	buffer[j] = *(temp_ptr+1);	j++;
	
	//Write time
	Capture(&chr);
	tmp = GetLastDelay_us(&chr);
	temp_ptr = (uint8_t*)(&tmp);
		
	buffer[j] = *temp_ptr;		j++;
	buffer[j] = *(temp_ptr+1);	j++;
	buffer[j] = *(temp_ptr+2);	j++;
	buffer[j] = *(temp_ptr+3);	j++;
	
	//Start frame. send CMD,DATAID,time to protocol
	protocol_frame_begin();
	protocol_frame_append(buffer,j);
	j = 0;
	
	//Write data
	for(k = 0 ; k < Log.variables[i].size ; k++)
	{
		temp_ptr = Log.variables[i].ptr + k;
		
		protocol_frame_append(temp_ptr,1);
	}
	protocol_frame_end();
	
	return 0;
}
