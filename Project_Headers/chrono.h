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

void Stop(chrono* chr);

uint32_t Get(chrono* chr);

#endif /* CHRONO_H_ */
