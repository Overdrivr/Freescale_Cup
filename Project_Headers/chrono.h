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

void Restart(chrono* chr);

void Capture(chrono* chr);

float GetLastDelay_us(chrono* chr);

float GetLastDelay_ms(chrono* chr);

#endif /* CHRONO_H_ */
