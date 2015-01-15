#ifndef TFC_UART_H_
#define TFC_UART_H_

#include "TFC/TFC_Queue.h"

void TFC_InitUARTs();

void TFC_UART_Process();

extern ByteQueue SDA_SERIAL_OUTGOING_QUEUE;
extern ByteQueue SDA_SERIAL_INCOMING_QUEUE;


#endif /* TFC_UART_H_ */
