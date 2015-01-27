/*
 * logger.h
 *
 *  Created on: Oct 13, 2014
 *      Author: B48923
 */

#ifndef DISTANTIO_H_
#define DISTANTIO_H_

//1 log can retain 128 different variables (=ptr adress) (A table only counts 1)
#include ".\TFC\TFC.h"
#include "protocol.h"

typedef enum datatype datatype;
enum datatype
{
	FLOAT = 0x00,
	UINT8 = 0x01,
	UINT16 = 0x02,
	UINT32 = 0x03,
	INT8 = 0x04,
	INT16 = 0x05,
	INT32 = 0x06,
};

typedef struct variable variable;
struct variable
{
	uint8_t* ptr;
	uint16_t size;
	uint8_t writeable;
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

void init_distantio();

uint8_t register_scalar(void* adress, 
					    datatype type, 
					    uint8_t writeable, 
					    char* name);

uint8_t register_array(void* adress,
					   uint16_t size,
					   datatype type, 
					   uint8_t writeable, 
					   char* name);

// To call as often as possible
void update_distantio();

// Feed a new RX frame to DistantIO
void distantio_decode_rx_frame(ByteQueue* rx_queue);

#endif /* DISTANTIO_H_ */
