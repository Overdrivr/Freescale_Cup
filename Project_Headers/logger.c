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
	for(i = 0 ; i < 128 ; i++)
	{
		Log.variables_size[i] = 0;
		Log.variables_ptr[i] = 0;
		Log.rw_rights[i] = 1;//1 means readonly
	}
		
}

//Returns 0 if ok, 1 if size exceeds log capacity
uint8_t add_to_log(log* table, uint8_t* adress, uint16_t octets, uint8_t readonly)
{
	if(Log.current_index == 127)
		return 1;
	
	//Limited to 1024 octets per variable, should be enough for now
	if(octets > 1024)
		return 1;
	
	Log.variables_ptr[Log.current_index] = adress;
	Log.variables_size[Log.current_index] = octets;
	Log.rw_rights[Log.current_index] = readonly;
	
	Log.current_index++;
	
	return 0;
}

void update_serial()
{
	int i,j, adress;
	uint8_t buffer;
	uint8_t header = 85;//01010101
	uint8_t footer = 204;//11001100
	uint8_t *temp_ptr;
	
	//Transmit data to serial
	for(i = 0 ; i < Log.current_index ; i++)
	{
		
		//Set header (2 octets)
		serial_write(header,1);
		serial_write(header,1);
		
		//Set framesize
		uint16_t framesize = Log.variables_size[i];
		temp_ptr = (uint8_t*)(&framesize);
		serial_write(*temp_ptr,1); 		
		serial_write(*(temp_ptr+1),1);
		
		//Set CMD
			//1 is return_log_table..TBD!
			//2 is return_log_values
		serial_write(2,1);
		
		//Set data
		for(j = 0 ; j < Log.variables_size[i] ; j++)
		{
			adress = i + j;
			buffer = *(Log.variables_ptr[adress]);
			serial_write(buffer,1);
		}
	
		
		//Set EOFrame
		serial_write(footer,1);		
	}
		
	
	//Read data from serial
	/*while(serial_available())
	{
		
	}*/
}
