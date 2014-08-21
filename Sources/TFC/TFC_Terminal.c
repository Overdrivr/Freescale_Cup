#include "TFC\TFC.h"


//*****************************************************************
//Terminal Configuration
//*****************************************************************

#define MAX_TERMINAL_LINE_CHARS 64
#define MAX_TERMINAL_CMD_CHARS  32


typedef void (*TerminalCallback)(char *);


typedef struct 
{
	const char *CommandString;
	TerminalCallback Callback;
	const char *HelpString;
		
} TerminalCallbackRecord;

//Callback function prototypes
void TerminalCmd_Help(char *arg);
void TerminalCmd_Reboot(char *arg);
void TerminalCmd_GetLineScan(char *arg);
void TerminalCmd_S(char *arg);
void TerminalCmd_H(char *arg);
void TerminalCmd_E(char *arg);
void TerminalCmd_D(char *arg);
//Populate this array with the callback functions and their terminal command string
TerminalCallbackRecord MyTerminalCallbackRecords[] ={	{"help",TerminalCmd_Help,"Lists available commands"},
                                                        {"L",TerminalCmd_GetLineScan,"Displays linescan camera data"},
                                                        {"S",TerminalCmd_S,"Commands the servos to a position"},
                                                        {"H", TerminalCmd_H,"Commands the H-Bridges"},
														{"E",TerminalCmd_E, "Enables the H-Bridges"},
														{"D",TerminalCmd_D, "Disables the H-Bridges"},
                                                        };

//*****************************************************************
//Plumbing.....
//*****************************************************************

#define NUM_TERMINAL_COMMANDS  (sizeof(MyTerminalCallbackRecords)/sizeof(TerminalCallbackRecord))

char TerminalLineBuf[MAX_TERMINAL_LINE_CHARS];
uint8_t TerminalPos;
char TerminalCmdBuf[MAX_TERMINAL_CMD_CHARS+1];
char TerminalArgs[MAX_TERMINAL_LINE_CHARS-MAX_TERMINAL_CMD_CHARS];
uint8_t NextCharIn;
uint8_t CmdFound;
 
void TerminalBootMsg()
{

TERMINAL_PRINTF("\r\n\r\n"); 
TERMINAL_PRINTF("***********************************\r\n"); 
TERMINAL_PRINTF("FRDM-TFC		           \r\n");
TERMINAL_PRINTF("Copyright (C) <2013>  Eli Hughes\r\n");
TERMINAL_PRINTF("Wavenumber LLC\r\n"); 
TERMINAL_PRINTF("***********************************\r\n\r\n>"); 

}

void TFC_InitTerminal()
{
	TerminalPos = 0;
	CmdFound = 0;
    TerminalBootMsg();
}

void TerminalCmd_Help(char *arg)
{
    uint8_t i;

    TERMINAL_PRINTF("\r\n\r\nCommand List:\r\n");
    TERMINAL_PRINTF("----------------------\r\n");

    for(i=0;i<NUM_TERMINAL_COMMANDS;i++)
    {
         TERMINAL_PRINTF("%s  ---->  %s\r\n",MyTerminalCallbackRecords[i].CommandString,MyTerminalCallbackRecords[i].HelpString);    
    }

    TERMINAL_PRINTF("\r\n\r\n");
}

void TerminalCmd_Reboot(char *arg)
{
      TerminalBootMsg();
}



void TerminalCmd_E(char *arg)
{
	 TERMINAL_PRINTF("\t\r\nEnabling H-Bridges\r\n");
	 TFC_HBRIDGE_ENABLE;
}

void TerminalCmd_D(char *arg)
{
	 TERMINAL_PRINTF("\t\r\nDisabling H-Bridges\r\n");
	 TFC_HBRIDGE_DISABLE;
	 TFC_SetMotorPWM(0,0);
}

void TerminalCmd_GetLineScan(char *arg)
{
	uint8_t i;
	
	TERMINAL_PRINTF("\r\nLINE 0:");
	
	for(i=0;i<128;i++)
	{
		
		TERMINAL_PRINTF("%2X",LineScanImage0[i]);
		
		if(i<127)
			TERMINAL_PRINTF(",");
		else
			TERMINAL_PRINTF("\r\n\r\n");
	}
	TERMINAL_PRINTF("\r\nLINE 1:");
	for(i=0;i<128;i++)
	{
		TERMINAL_PRINTF("%2X",LineScanImage1[i]);
		if(i<127)
					TERMINAL_PRINTF(",");
				else
					TERMINAL_PRINTF("\r\n\r\n");
	}

}


void TerminalCmd_S(char *arg)
{
	int ServoSetting[2] = {0,0};

	if(sscanf(arg,"%d,%d",&ServoSetting[0],&ServoSetting[1]) == 2)
	{
	
	TERMINAL_PRINTF("Setting Servos to %d , %d\r\n",ServoSetting[0],ServoSetting[1]);
	
	TFC_SetServo(0,(float)ServoSetting[0]/100.0f); //Rescale to -1.0 to 1.0
	TFC_SetServo(1,(float)ServoSetting[1]/100.0f);
	}
	else
	{
		TERMINAL_PRINTF("Invalid servo control string. There must be 2 command separated arguments between -100 and 100. Ex: S 43,-43");
	}
	
}

void TerminalCmd_H(char *arg)
{

	int MotorSetting[2] = {0,0};

	if(sscanf(arg,"%d,%d",&MotorSetting[0],&MotorSetting[1]) == 2)
	{
	
	TERMINAL_PRINTF("Setting motor effort to to %d , %d\r\n",MotorSetting[0],MotorSetting[1]);
	
	TFC_SetMotorPWM((float)MotorSetting[0]/100.0f,(float)MotorSetting[1]/100.0f);


	}
	else
	{
		TERMINAL_PRINTF("Invalid motor control string. There must be 2 command separated arguments between -100 and 100. Ex: H 43,-43\r\n");
	}
	
}


void TFC_ProcessTerminal()
{
     uint8_t i,j;
     uint8_t ArgsFound;
        
    if(TERMINAL_READABLE)
    {
       NextCharIn = TERMINAL_GETC;
       
        switch(NextCharIn)
        {
            case '\r':
             
             TerminalLineBuf[TerminalPos++] = 0x0;
             TERMINAL_PUTC(NextCharIn);
           
             if(TerminalPos > 1)
             {
                 //find the command
                 i=0;
                 while(TerminalLineBuf[i]>0x20 &&  TerminalLineBuf[i]<0x7f)
                 {
                      TerminalCmdBuf[i] = TerminalLineBuf[i];
                      i++;
    
                    if(i==MAX_TERMINAL_CMD_CHARS)
                        {
                         break;
                        }
                 }
                    
                TerminalCmdBuf[i] = 0;
                TerminalCmdBuf[i+1] = 0;
                
                
                ArgsFound = TRUE;
                memset(TerminalArgs,0x00,sizeof(TerminalArgs));
                //scan for num terminator or next non whitespace
                while(TerminalLineBuf[i]<=0x20 && (i<MAX_TERMINAL_LINE_CHARS))
                {
                    if(TerminalLineBuf[i] == 0x00)
                    {
                    
                        //if we find a NULL terminator before a non whitespace character they flag for no arguments
                        ArgsFound = FALSE;
                        break;
                    }   
                    i++; 
                }
                
                if(ArgsFound == TRUE)
                {
                    strcpy(TerminalArgs,&TerminalLineBuf[i]);
                    
                    //trim trailing whitespace
                    i = sizeof(TerminalArgs)-1;
                    
                    while((TerminalArgs[i]<0x21) && (i>0))
                    {
                        TerminalArgs[i]= 0x00;
                        i--;
                    }       
                }
                
                CmdFound = FALSE;
                for(j=0;j<NUM_TERMINAL_COMMANDS;j++)
                {           
                    if(strcmp(TerminalCmdBuf,MyTerminalCallbackRecords[j].CommandString) == 0)
                    {
                        TERMINAL_PRINTF("\r\n");
                        if(MyTerminalCallbackRecords[j].Callback != NULL)
                            MyTerminalCallbackRecords[j].Callback(TerminalArgs);
                    
                        CmdFound = TRUE;
                        break;
                    }             
                }        
                if(CmdFound == FALSE)
                {
                  TERMINAL_PRINTF("\r\n%s command not recognized.\r\n\r\n",TerminalCmdBuf);
                  TerminalCmd_Help("no arg");
                  
                }
              }    
             TERMINAL_PRINTF("\r\n>");
             TerminalPos = 0;
            
            break;
            
            case '\b':
                if(TerminalPos > 0)
                {
                    TerminalPos--;    
                    TERMINAL_PUTC(NextCharIn);
                }
            break;
            
            default:
                
                if(TerminalPos == 0 && NextCharIn == 0x020)
                {
                     //Do nothing if space bar is pressed at beginning of line    
                }
                   else if(NextCharIn >= 0x20 && NextCharIn<0x7F)
                {
                    
                    if(TerminalPos < MAX_TERMINAL_LINE_CHARS-1)
                    {
                         TerminalLineBuf[TerminalPos++] = NextCharIn;
                        TERMINAL_PUTC(NextCharIn);
                    }
                }
            
            break;
        
        }
    }
 
}


