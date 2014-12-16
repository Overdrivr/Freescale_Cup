/*
 * chrono.c
 *
 *  Created on: Dec 15, 2014
 *      Author: B48923
 */

#include "chrono.h"

void Restart(chrono* chr)
{
	chr->start = TFC_Ticker[6];
}

void Stop(chrono* chr)
{
	chr->stop = TFC_Ticker[6];
	chr->duration = chr->stop - chr->start;
}

uint32_t Get(chrono* chr)
{
	return chr->duration;
}
