/*
 * logger.c
 *
 *  Created on: Oct 13, 2014
 *      Author: B48923
 */

#include "logger.h"

uint8_t buffer[SDA_SERIAL_OUTGOING_QUEUE_SIZE];

void init_log(log* Log)
{
	uint16_t i;
	char default_name[] = {"undefined"};
	Log->current_index = 0;
	for(i = 0 ; i < 128 ; i++)
	{
		Log->variables[i].size = 0;
		Log->variables[i].ptr = 0;
		Log->variables[i].rw_rights = 1;//1 means readonly
		Log->variables[i].id = i;
		strcpy(Log->variables[i].name,default_name);
	}
		
}

//Returns 0 if ok, 1 if size exceeds log capacity
uint8_t add_to_log(log* Log, uint8_t* adress, uint16_t octets, datatype type, uint8_t readonly, char* name)
{
	if(Log->current_index == 127)
		return 1;
	
	//Limited to 1024 octets per variable, should be enough for now
	if(octets > 1024)
		return 1;
	
	Log->variables[Log->current_index].ptr = adress;
	Log->variables[Log->current_index].size = octets;
	Log->variables[Log->current_index].rw_rights = readonly;
	Log->variables[Log->current_index].type = type;
	strcpy(Log->variables[Log->current_index].name,name);
	
	Log->current_index++;
	
	return 0;
}

void update_log_serial(log* Log)
{
	uint16_t i,j,k;
	uint8_t *temp_ptr;
	uint8_t type;
	
	//Transmit data to serial
	for(i = 0 ; i < Log->current_index ; i++)
	{/*
		j = 0;
		
		//Write command
		buffer[j] = 0x00;			j++;
				
		//Write data type
		switch(Log->variables[i].type)
		{
			case FLOAT:
				type = 1;
				break;
				
			case INT32:
			default:
				type = 2;
				break;
		}
		buffer[j] = type;			j++;
		
		//Write variable ID
		temp_ptr = (uint8_t*)(&i);
		buffer[j] = *temp_ptr;		j++;
		buffer[j] = *(temp_ptr+1);	j++;
			
		//Write data
		for(k = 0 ; k < Log->variables[i].size ; k++)
		{
			buffer[j] = (*Log->variables[i].ptr + k);
			j++;
		}*/
			
	
		//Send to serial protocol
		//send_serial_frame(buffer,j);
		serial_printf("TEST");
		//serial_printf("%s\n",buffer);
	}
}
