#ifndef TFC_ARM_SYSTICK_H_
#define TFC_ARM_SYSTICK_H_

void TFC_InitSysTick();
void TFC_Delay_mS(unsigned int TicksIn_mS);
void TFC_SysTickIrq();

#define SYSTICK_FREQUENCY 1000

extern volatile uint32_t TFC_Ticker[NUM_TFC_TICKERS];

#endif /* SYSTICK_H_ */

