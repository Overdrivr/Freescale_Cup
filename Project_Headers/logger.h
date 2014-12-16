/*
 * logger.h
 *
 *  Created on: Oct 13, 2014
 *      Author: B48923
 */

#ifndef LOGGER_H_
#define LOGGER_H_

//1 log can retain 128 different variables (=ptr adress) (A table only counts 1)
#include "TFC\TFC.h"
#include "serial_protocol.h"

typedef enum datatype datatype;
enum datatype
{
	FLOAT,
	INT32,
	INT16,
	INT8,
	UINT32,
	UINT16,
	UINT8
	
};

typedef struct variable variable;
struct variable
{
	uint8_t* ptr;
	uint16_t size;
	uint8_t rw_rights;
	uint16_t id;
	datatype type;
	char name[32];
	uint8_t send;
};

typedef struct log log;
struct log
{
	variable variables[128];
	uint16_t current_index;
	uint16_t previous_index;
};

void init_log();

//Returns 0 if ok, 1 if size exceeds log capacity
uint8_t add_to_log(uint8_t* adress, 
				   uint16_t octets,
				   datatype type, 
				   uint8_t readonly, 
				   char* name);

void update_log_serial();

void log_process_serial(ByteQueue* rx_queue);

void send_table();

#endif /* LOGGER_H_ */
