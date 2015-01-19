/*
 * UnitTests.h
 *
 *  Created on: Jan 14, 2015
 *      Author: B48923
 */

#ifndef UNITTESTS_H_
#define UNITTESTS_H_

#include "TFC/TFC_UART.h"
#include "TFC/TFC.h"
#include "Serial\serial.h"
#include "chrono.h"
#include "DistantIO\distantio.h"

// Self-contained test functions
// To call from main without any other code

/*
 * test_serial1 : To run with UnitTest_Serial.py
 * Outputs a define sequence on the UART that UnitTest_Serial.py will receive and check during 150s
 */
void test_serial1();

/*
 * test_protocol1 : To run with UnitTest_Protocol.py
 * Outputs manually a distantio frame on the UART that UnitTest_Protocol checks
 */
void test_protocol1();

/*
 * test_distantio_minimal : To run with UnitTest_DistantIORead
 * Registers a single variable and let distantio deal with it. 
 * Validates byte stuffing destuffing
 * 
 */
void test_distantio_minimal();

#endif /* UNITTESTS_H_ */
