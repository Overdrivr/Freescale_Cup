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
	chr->duration = 0;
}

void update(chrono* chr)
{
	chr->stop = TFC_Ticker[0];
	chr->duration = chr->stop - chr->start;
}

float us(chrono* chr)
{
	return chr->duration * 1000.f * 1000.f / (float)(SYSTICK_FREQUENCY);
}

float ms(chrono* chr)
{
	return chr->duration * 1.f * 1000.f / (float)(SYSTICK_FREQUENCY);
}
