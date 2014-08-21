#include "TFC\TFC.h"


//set I/O for H-BRIDGE enables, switches and LEDs
void TFC_InitGPIO()
{
	//enable Clocks to all ports
	
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

	//Setup Pins as GPIO
	PORTE_PCR21 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   
	PORTE_PCR20 = PORT_PCR_MUX(1);    
	
	//Port for Pushbuttons
	PORTC_PCR13 = PORT_PCR_MUX(1);   
	PORTC_PCR17 = PORT_PCR_MUX(1);   
	
	
	//Ports for DIP Switches
	PORTE_PCR2 = PORT_PCR_MUX(1); 
	PORTE_PCR3 = PORT_PCR_MUX(1);
	PORTE_PCR4 = PORT_PCR_MUX(1); 
	PORTE_PCR5 = PORT_PCR_MUX(1);
	
	//Ports for LEDs
	PORTB_PCR8 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   
	PORTB_PCR9 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   
	PORTB_PCR10 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   
	PORTB_PCR11 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   
	
	
	//Setup the output pins
    GPIOE_PDDR =  TFC_HBRIDGE_EN_LOC;  
    GPIOB_PDDR =  TFC_BAT_LED0_LOC	| TFC_BAT_LED1_LOC | TFC_BAT_LED2_LOC | TFC_BAT_LED3_LOC;

    TFC_SetBatteryLED_Level(0);
    TFC_HBRIDGE_DISABLE;
    
}


void TFC_SetBatteryLED_Level(uint8_t BattLevel)
{
	switch(BattLevel)
	{
		default:
		case 0:
			GPIOB_PCOR = 0xF<<8;
		break;
	
		case 1:
			GPIOB_PSOR = 0x01<<8;
			GPIOB_PCOR = ~(0x01<<8) & (0xF<<8);
		break;
		
		case 2:
			GPIOB_PSOR = 0x03<<8;
			GPIOB_PCOR = ~(0x03<<8) & (0xF<<8) ;
		break;
		
		case 3:
			GPIOB_PSOR = 0x07<<8;
			GPIOB_PCOR = ~(0x07<<8) & (0xF<<8);
		break;
		
		case 4:
			GPIOB_PSOR = 0x0F<<8;
		break;
		
	}
}


uint8_t TFC_GetDIP_Switch()
{
	uint8_t DIP_Val=0;
	
	DIP_Val = (GPIOE_PDIR>>2) & 0xF;

	return DIP_Val;
}
