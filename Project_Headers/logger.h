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

typedef struct log log;
struct log
{
	uint8_t* variables_ptr[128];
	uint16_t variables_size[128];
	uint8_t rw_rights[128];
	uint16_t current_index;
};

extern log Log;

void init_log();

//Returns 0 if ok, 1 if size exceeds log capacity
uint8_t add_to_log(log* table, uint8_t* adress, uint16_t octets, uint8_t readonly);

void update_log_serial();

#endif /* LOGGER_H_ */
