/*
 * UnitTests.c
 *
 *  Created on: Jan 14, 2015
 *      Author: B48923
 */
#include "UnitTests.h"

void test_serial1()
{
	TFC_Init();
	init_serial();
	
	chrono chr;
	
	uint8_t  testESC[10];
	testESC[0] = 0x00;
	testESC[1] = 0x01;
	testESC[2] = 0x02;
	testESC[3] = 0x03;
	testESC[4] = 0x04;
	testESC[5] = 0x05;
	testESC[6] = 0x06;
	testESC[7] = 0x07;
	testESC[8] = 0x08;
	testESC[9] = 0x09;
		
	Restart(&chr);
	
	for(;;)
	{
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		TFC_Task();
		
		Capture(&chr);
		if(GetLastDelay_us(&chr) > 1500)
		{
			Restart(&chr);
			serial_write(testESC,10);
		}			
		
	}
}

void test_protocol1()

{
	TFC_Init();
	init_serial();
	
	chrono chr;
	
	// data sent : 0x007F7DF7;
	//For testing all ESC characters
	
	uint8_t  testESC[13];
	testESC[0] = 0xf7;
	testESC[1] = 0x00;
	testESC[2] = 0x03;
	testESC[3] = 0x00;
	testESC[4] = 0x00;
		testESC[5] = 0x7D;
	testESC[6] = 0xf7;
		testESC[7] = 0x7D;
	testESC[8] = 0x7D;
		testESC[9] = 0x7D;
	testESC[10] = 0x7f;
	testESC[11] = 0x00;
	testESC[12] = 0x7f;
	/*
	uint8_t  testESC[10];
	testESC[0] = 0xf7;
	testESC[1] = 0x00;
	testESC[2] = 0x03;
	testESC[3] = 0x00;
	testESC[4] = 0x00;
	testESC[5] = 0x00;
	testESC[6] = 0x00;
	testESC[7] = 0x00;
	testESC[8] = 0x00;
	testESC[9] = 0x7f;
	*/
		
	Restart(&chr);
	
	for(;;)
	{
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		TFC_Task();
		
		Capture(&chr);
		if(GetLastDelay_us(&chr) > 1500)
		{
			Restart(&chr);
			serial_write(testESC,13);
		}			
		
	}
}

void test_distantio_minimal()
{
	TFC_Init();	
	
	init_serial();
	init_protocol();
	init_distantio();
	
	//TFC_HBRIDGE_ENABLE;
	
	uint32_t testESC =  0x007F7DF7;
	
	//Readonly variables
	register_scalar(&testESC,UINT32,0,"TestESC");
	
	chrono chr_distantio;
	Restart(&chr_distantio);
	
	for(;;)
	{
			
		//TFC_Task must be called in your main loop.  This keeps certain processing happy (I.E. Serial port queue check)
		TFC_Task();
		
		Capture(&chr_distantio);
		if(GetLastDelay_us(&chr_distantio) > 500)
		{
			Restart(&chr_distantio);
				
			update_distantio();
		}			
	}
}
