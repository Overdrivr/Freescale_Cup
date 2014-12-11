#Freescale_Cup

Autonomous vehicle competition using Freescale hardware
https://community.freescale.com/groups/tfc-emea
https://community.freescale.com/docs/DOC-1284

This repository contains :

1. MCU side
  * Base code for the MCU provided by Freescale (Licence for this code is unknown, I do claim any rights on it)
  This code is located in /Project_Headers/TFC/ and /Sources/TFC/
  * Custom MCU code to process linescan camera, compute car position on the road and give commands to the direction/motors 
  * Logger files (It is actually not a logger, simply a module that processes serial input on the MCU side and changes variable values/returns variable values to the computer)
  The code is in serial.h serial.c logger.h logger.c serial_protocol.h serial_protocol.c)

2. Computer side
![gui screenshot](https://raw.githubusercontent.com/Overdrivr/Freescale_Cup/master/gui.png) "GUI Screenshot")
  Advanced logging & debugging interface that communicates with the MCU 'logger' to write to/read variables on the MCU in 'real' time
  Protocols are defined in serial_protocols_definition.xlsx
  Python program is located /Python/ and the file to execute is GUI.py. Extension modules are required, see below for more information

## Hardware

"Freedom FRDM-KL25Z board" - Development platform for Freescale Kinetis L series MCU (Processor ARM Cortex M0+)
http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=FRDM-KL25Z

"FRDM-TFC" - Control Shield 
https://community.freescale.com/docs/DOC-93914

## Software
CodeWarrior for MCU development & debug
Usb serial port drivers are available at : http://www.pemicro.com/opensda/


Python for real time data plot

## Python 
### Core version
Python 3.4.2 minimum (https://www.python.org/downloads/)

### Modules to install (manually or via pip)
* matplotlib 1.4.2 
* numpy 1.9.0 minimum
* scipy 0.14 minimum
* pyserial
* pyttk
* PyPubSub
