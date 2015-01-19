#include "TFC\TFC.h"

#ifndef TFC_ADC_H_
#define TFC_ADC_H_

void TFC_InitADCs();

void PIT0_IRQ();
void ADC0_IRQ();

float TFC_ReadPot(uint8_t Channel);
float TFC_ReadBatteryVoltage();

#endif /* __ADC16_H__ */

