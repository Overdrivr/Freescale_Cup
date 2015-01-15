/*
 * serial.h
 *
 *  Created on: Oct 13, 2014
 *      Author: B48923
 */

// Copyright (C) 2014 Rémi Bèges
// For conditions of distribution and use, see copyright notice in the LICENSE file

#ifndef SERIAL_H_
#define SERIAL_H_

#include "..\TFC\TFC.h"
#include "..\TFC\TFC_UART.h"
#include "..\TFC\TFC_Queue.h"

//Similar interface to arduino

//QUEUES FOR RECEIVE/TRANSMIT
#define SERIAL_INCOMING_QUEUE SDA_SERIAL_INCOMING_QUEUE
#define SERIAL_OUTGOING_QUEUE SDA_SERIAL_OUTGOING_QUEUE

//Returns stats about serial
uint16_t getPeakLoad();

//Init serial (reset stats)
void init_serial();

//Write 'len' bytes of data in 'buf'
//Returns number of bytes written
int16_t serial_write(uint8_t* buf, uint16_t len);

//Returns the space in the tx queue
uint16_t serial_write_available();

#define serial_printf(...) Qprintf(&SERIAL_OUTGOING_QUEUE,__VA_ARGS__)

//Returns number of received bytes in the reception queue (basically nb of bytes that can be read)
uint16_t serial_available();

//Returns first byte of received data
uint8_t serial_read();


#endif /* SERIAL_H_ */
