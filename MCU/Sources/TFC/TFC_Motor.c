#include "TFC\TFC.h"
#include "derivative.h"


#define FTM0_MOD_VALUE	(int)((float)(PERIPHERAL_BUS_CLOCK)/TFC_MOTOR_SWITCHING_FREQUENCY)

#define FTM0_CLOCK                                   	      (CORE_CLOCK/2)
#define FTM0_CLK_PRESCALE                                 	   0  // Prescale Selector value - see comments in Status Control (SC) section for more details
#define FTM0_OVERFLOW_FREQUENCY 5000							  //


/**********************************************************************************************/


void TFC_InitMotorPWM()
{
	//Clock Setup for the TPM requires a couple steps.

	
    //1st,  set the clock mux
    //See Page 124 of f the KL25 Sub-Family Reference Manual, Rev. 3, September 2012
    SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;// We Want MCGPLLCLK/2 (See Page 196 of the KL25 Sub-Family Reference Manual, Rev. 3, September 2012)
    SIM_SOPT2 &= ~(SIM_SOPT2_TPMSRC_MASK);
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1); //We want the MCGPLLCLK/2 (See Page 196 of the KL25 Sub-Family Reference Manual, Rev. 3, September 2012)


	//Enable the Clock to the FTM0 Module
	//See Page 207 of f the KL25 Sub-Family Reference Manual, Rev. 3, September 2012
	SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;
    
    //The TPM Module has Clock.  Now set up the peripheral
    
    //Blow away the control registers to ensure that the counter is not running
    TPM0_SC = 0;
    TPM0_CONF = 0;
    
    //While the counter is disabled we can setup the prescaler
    
    TPM0_SC = TPM_SC_PS(FTM0_CLK_PRESCALE);
    
    //Setup the mod register to get the correct PWM Period
    
    TPM0_MOD = FTM0_CLOCK/(1<<FTM0_CLK_PRESCALE)/FTM0_OVERFLOW_FREQUENCY;
    
    //Setup Channels 0,1,2,3
    TPM0_C0SC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    TPM0_C1SC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK; // invert the second PWM signal for a complimentary output;
    TPM0_C2SC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    TPM0_C3SC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK; // invert the second PWM signal for a complimentary output;
    
    
    //Enable the Counter
    
    //Set the Default duty cycle to 50% duty cycle
    TFC_SetMotorPWM(0.0,0.0);
    
    //Enable the TPM COunter
    TPM0_SC |= TPM_SC_CMOD(1);
    
    
    //Enable the FTM functions on the the port
    PORTC_PCR1 = PORT_PCR_MUX(4);
    PORTC_PCR2 = PORT_PCR_MUX(4);     
    PORTC_PCR3 = PORT_PCR_MUX(4);  
    PORTC_PCR4 = PORT_PCR_MUX(4);  

}



void TFC_SetMotorPWM(float MotorA , float MotorB)
{
	
	if(MotorA>1.0)
		MotorA = 1.0;
	else if(MotorA<-1.0)
		MotorA = -1.0;
	
	if(MotorB>1.0)
			MotorB = 1.0;
		else if(MotorB<-1.0)
			MotorB = -1.0;
		
	TPM0_C2V = (uint16_t) ((float)TPM0_MOD * (float)((MotorA + 1.0)/2.0));
	TPM0_C3V = TPM0_C2V;
	TPM0_C0V = (uint16_t) ((float)TPM0_MOD * (float)((MotorB + 1.0)/2.0));
	TPM0_C1V = TPM0_C0V;

}


