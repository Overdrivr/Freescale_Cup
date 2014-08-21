#include "TFC\TFC_Config.h"
#include "TFC\TFC_UART.h"

#ifndef TFC_TERMINAL_H_
#define TFC_TERMINAL_H_

void TFC_InitTerminal();
void TFC_ProcessTerminal();

#if defined(TERMINAL_USE_SDA_SERIAL)
	#define TERMINAL_OUT_QUEUE			SDA_SERIAL_OUTGOING_QUEUE
	#define TERMINAL_IN_QUEUE			SDA_SERIAL_INCOMING_QUEUE
	#define TERMINAL_PRINTF(...)   		Qprintf(&TERMINAL_OUT_QUEUE,__VA_ARGS__)  
	#define TERMINAL_PUTC(c)        	ByteEnqueue(&TERMINAL_OUT_QUEUE,c)
	#define TERMINAL_READABLE       	BytesInQueue(&TERMINAL_IN_QUEUE)
	#define TERMINAL_GETC           	ForcedByteDequeue(&TERMINAL_IN_QUEUE)
#else
	#error "Unsupported Terminal Configuration!"
#endif



#endif /* TFC_TERMINAL_H_ */
