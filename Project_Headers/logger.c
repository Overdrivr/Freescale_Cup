/*
 * logger.c
 *
 *  Created on: Oct 13, 2014
 *      Author: B48923
 */

#include "logger.h"
#include "serial.h"


void init_log()
{
	int i;
	Log.current_index = 0;
	char default_name[] = {"undefined"};
	for(i = 0 ; i < 128 ; i++)
	{
		Log.variables[i].size = 0;
		Log.variables[i].ptr = 0;
		Log.variables[i].rw_rights = 1;//1 means readonly
		Log.variables[i].id = i;
		strcpy(Log.variables[i].name,default_name);
	}
		
}

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
	int i,j, adress;
	uint8_t buffer;
	uint8_t header = 85;//01010101
	uint8_t footer = 204;//11001100
	uint8_t *temp_ptr;
	uint8_t type = 1;
	
	//Transmit data to serial
	for(i = 0 ; i < Log.current_index ; i++)
	{
		
		//Write header
		serial_write(&header,1);
		
		//Write data size
		uint16_t datasize = Log.variables[i].size;
		temp_ptr = (uint8_t*)(&datasize);
		serial_write(temp_ptr,2); 		
		
		//Write data type
		switch(Log.variables[i].type)
		{
			case FLOAT:
				type = 1;
				break;
			case INT32:
				type = 2;
				break;
		}
		serial_write(&type,1);
		
		//Write variable ID
		temp_ptr = (uint8_t*)(&datasize);
		serial_write(temp_ptr,2);
		
		//Write command
		type = 15;//b'0000 1111 - variable value answer
		serial_write(&type,1);
		
		//Write data
		serial_write(Log.variables[i].ptr,Log.variables[i].size);
	
		//Set EOFrame
		serial_write(footer,1);		
	}
}
