/*
 * chrono.c
 *
 *  Created on: Dec 15, 2014
 *      Author: B48923
 */

#include "chrono.h"
#include "TFC/TFC_ARM_SysTick.h"

void reset(chrono* chr)
{
	chr->start = TFC_Ticker[0];
}

float us(chrono* chr)
{
	chr->stop = TFC_Ticker[0];
	return (chr->stop - chr->start) * 1000.f * 1000.f / (float)(SYSTICK_FREQUENCY);
}

float ms(chrono* chr)
{
	chr->stop = TFC_Ticker[0];
	return (chr->stop - chr->start) * 1000.f / (float)(SYSTICK_FREQUENCY);
}
