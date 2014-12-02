/*
 * logger.c
 *
 *  Created on: Oct 13, 2014
 *      Author: B48923
 */

#include "logger.h"
#include "serial.h"

log Log;

void init_log()
{
	uint16_t i;
	char default_name[] = {"undefined"};
	Log.current_index = 0;
	Log.previous_index = 0;
	for(i = 0 ; i < 128 ; i++)
	{
		Log.variables[i].size = 0;
		Log.variables[i].ptr = 0;
		Log.variables[i].rw_rights = 1;//1 means readonly
		Log.variables[i].id = i;
		strcpy(Log.variables[i].name,default_name);
		Log.variables[i].send = 0;
	}
		
}

//TODO : Replace octets by array_size ?
//Returns 0 if ok, 1 if size exceeds log capacity
uint8_t add_to_log(uint8_t* adress, uint16_t octets, datatype type, uint8_t readonly, char* name)
{
	if(Log.current_index == 127)
		return 1;
	
	//Limited to 1024 octets per variable, should be enough for now
	if(octets > 1024)
		return 1;
	
	Log.variables[Log.current_index].ptr = adress;
	Log.variables[Log.current_index].size = octets;
	Log.variables[Log.current_index].rw_rights = readonly;
	Log.variables[Log.current_index].type = type;
	strcpy(Log.variables[Log.current_index].name,name);
	
	Log.current_index++;
	
	return 0;
}

void update_log_serial()
{
	
	uint16_t i,j,k;
	uint8_t *temp_ptr;
	uint8_t type;
	
	uint8_t buffer[512];
	
	uint16_t interval = 1;
	
	//Transmit data to serial - Only one variable per mainloop
	for(i = Log.previous_index ; i < Log.previous_index + interval ; i++)
	{		
		if(Log.variables[i].send == 0)
			continue;
		
		j = 0;
		
		//Write command
		buffer[j] = 0x00;			j++;
				
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
		buffer[j] = type;			j++;
		
		//Write variable ID
		temp_ptr = (uint8_t*)(&i);
		serial_printf("%d",*temp_ptr);
		buffer[j] = *temp_ptr;		j++;
		buffer[j] = *(temp_ptr+1);	j++;
			
		//Write data
		for(k = 0 ; k < Log.variables[i].size ; k++)
		{
			buffer[j] = *(Log.variables[i].ptr + k);
			j++;
		}
					
		//Send to serial protocol
		send_serial_frame(buffer,j);
	}
	
	Log.previous_index += interval;
	if(Log.previous_index == Log.current_index)
	{
		Log.previous_index = 0;
	}
}


void log_process_serial(ByteQueue* rx_queue)
{
	//Get payload byte by byte
	
	uint8_t byte = ForcedByteDequeue(rx_queue);
	uint8_t byte2;
	uint16_t id;
	
	//If command is return table to master
	if(byte == 0x02)
	{
		send_table();
	}
	//If command is write variable value
	else if(byte == 0x01)
	{
		
	}
	//If command is return variable
	else if(byte == 0x00 && BytesInQueue(rx_queue) >= 3)
	{
		byte = ForcedByteDequeue(rx_queue);
		byte2 = ForcedByteDequeue(rx_queue);
		
		id = byte;
		id = (id << 4) + byte2;
		
		if(id < Log.current_index)
		{
			Log.variables[id].send = 1;
		}
	}
	
	//Clean queue
	while(BytesInQueue(rx_queue))
		ForcedByteDequeue(rx_queue);
}


void send_table()
{
	uint16_t i,j;
	uint8_t *temp_ptr;
	uint8_t type;
	
	uint8_t buffer[512];
	j = 0;
	
	//Return table command
	buffer[j] = 0x02;			j++;
	buffer[j] = 0x07;			j++;
	//Ignored
	buffer[j] = 0x00;			j++;
	buffer[j] = 0x00;			j++;
		
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
		if(Log.variables[i].rw_rights == 0)
			type += 0xF0;
		
		buffer[j] = type;			j++;
		
		//Data id
		temp_ptr = (uint8_t*)(&i);
		buffer[j] = *temp_ptr;		j++;
		buffer[j] = *(temp_ptr+1);	j++;
		
		temp_ptr = (uint8_t*)(&(Log.variables[i].size));
		buffer[j] = *temp_ptr;		j++;
		buffer[j] = *(temp_ptr+1);	j++;
		
		//Write name
		uint8_t k = 0;
		
		while(Log.variables[i].name[k] != '\0' || k < 32)
		{
			if(k < strlen(Log.variables[i].name))
				buffer[j] = Log.variables[i].name[k];
			else
				buffer[j] = 0;
			j++;
			k++;
		}		
	
	}
	//Send to serial protocol
	send_serial_frame(buffer,j);
}
