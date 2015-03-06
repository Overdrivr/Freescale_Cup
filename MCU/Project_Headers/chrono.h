/*
 * chrono.h
 *
 *  Created on: Dec 15, 2014
 *      Author: B48923
 */

#ifndef CHRONO_H_
#define CHRONO_H_

#include "TFC\TFC.h"

struct chrono
{
	uint32_t start;
	uint32_t stop;
};
typedef struct chrono chrono;

void reset(chrono* chr);

float us(chrono* chr);

float ms(chrono* chr);

//void remove_us(chrono* chr, uint32_t delay);

//void remove_ms(chrono* chr, uint32_t delay);

#endif /* CHRONO_H_ */
