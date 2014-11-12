import random
import sys
from threading import Thread
import time
import serial
from queue import Queue
from serial.tools.list_ports import comports
from enum import Enum
from pubsub import pub

class RX_STATE(Enum):
    IDLE = 0
    IN_PROCESS = 1
    
class ESC_STATE(Enum):
    IDLE = 0
    NEXT = 1

#Robust serial protocol with bit stuffing
#TODO : Use publish suscribe instead of callback for full frame decoded
class SerialProtocol():

    def __init__(self):
        self.rx_state = RX_STATE.IDLE;
        self.escape_state = ESC_STATE.IDLE;
        self.SOF = int('7f',16)
        self.EOF = int('7f',16)
        self.ESC = int('7d',16)
        self.payload = bytearray() 
        pub.subscribe(self.new_rx_byte,"new_rx_byte")

    def new_rx_byte(self, newbyte):
        #No frame in process
        if self.rx_state == RX_STATE.IDLE:
            if newbyte == self.SOF:
                # New frame started
                self.rx_state = RX_STATE.IN_PROCESS
            else:
                pub.publish('new_ignored_rx_byte',newbyte)
                
        #Frame is in process        
        else:
            #Next char must be data
            if self.escape_state == ESC_STATE.NEXT:
                #Byte destuffing, this char must not be interpreted as flag
                #See serial_protocols_definition.xlsx
                self.payload.append(newbyte)
                self.escape_state = ESC_STATE.IDLE
                
                
            #Next char can be data or flag (EOF, SOF,..)    
            elif self.escape_state == ESC_STATE.IDLE:
                #End of frame, the payload is immediatly send to callback function
                if newbyte == self.EOF:
                    self.publish("new_rx_payload",self.payload)
                    self.payload = bytearray()
                    self.rx_state = RX_STATE.IDLE
                    
                #Escaping
                elif newbyte == self.ESC:
                    self.escape_state = ESC_STATE.NEXT
                #Storing data
                else:
                    self.payload.append(newbyte)

    def process_tx_payload(self, payload):
        frame = bytearray()
        frame.append(self.SOF)
        
        for c in payload:
            if c == self.SOF or c == self.EOF or c == self.ESC:
                frame.append(self.ESC)
            frame.append(c)
                           
        frame.append(self.EOF)
        
        return frame
        

