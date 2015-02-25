#Freescale Cup

The Freescale Cup is an autonomous vehicle competition using Freescale hardware.

*_Note_* : This code does not complies with the new rules of the competition and thus cannot be used

## Authors
Rémi Bèges (Base code + GUI)

### Car program
  * Base code for the MCU provided by Freescale (Licence for this code is unknown, I do claim any rights on it)
  This code is located in /Project_Headers/TFC/ and /Sources/TFC/
  * Custom MCU code to process linescan camera, compute car position on the road and give commands to the direction/motors 

## Hardware
### MCU board
"Freedom FRDM-KL25Z board" - Development platform for Freescale Kinetis L series MCU (Processor ARM Cortex M0+)
http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=FRDM-KL25Z

### Peripheral shield
"FRDM-TFC" - Control Shield 
https://community.freescale.com/docs/DOC-93914

## Software
### MCU development
CodeWarrior Special Edition for MCU development (10.6 or more, Eclipse, offline) :
http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=CW-SUITE-SPECIAL&fpsp=1&tab=Design_Tools_Tab

### Serial port drivers
Usb serial port drivers (PEDrivers_install.exe) are available at :
http://www.pemicro.com/opensda/

### DistantIO
This code uses the DistantIO framework (https://github.com/Overdrivr/DistantIO) to read and write variables in real time. 
