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
	uint32_t duration;
};
typedef struct chrono chrono;

void reset(chrono* chr);

void update(chrono* chr);

float us(chrono* chr);

float ms(chrono* chr);

#endif /* CHRONO_H_ */
