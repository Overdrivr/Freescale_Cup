/*
 * chrono.c
 *
 *  Created on: Dec 15, 2014
 *      Author: B48923
 */

#include "chrono.h"
#include "TFC/TFC_ARM_SysTick.h"

void Restart(chrono* chr)
{
	chr->start = TFC_Ticker[0];
}

void Capture(chrono* chr)
{
	chr->stop = TFC_Ticker[0];
	chr->duration = chr->stop - chr->start;
}

float GetLastDelay_us(chrono* chr)
{
	return chr->duration * 1000000.f / (float)(SYSTICK_FREQUENCY);
}

float GetLastDelay_ms(chrono* chr)
{
	return chr->duration * 1000.f / (float)(SYSTICK_FREQUENCY);
}
