import random
import sys
from threading import Thread
import time
import serial
from queue import Queue
from serial.tools.list_ports import comports
from enum import Enum

class RX_STATE(Enum):
    IDLE = 0
    IN_PROCESS = 1
    
class ESC_STATE(Enum):
    IDLE = 0
    NEXT = 1

#Robust serial protocol with bit stuffing
class SerialProtocol():

    def __init__(self,fullframe_callback):
        self.rx_state = RX_STATE.IDLE;
        self.escape_state = ESC_STATE.IDLE;
        self.SOF = 0x7F
        self.EOF = 0x7F
        self.ESC = 0x7D
        self.frame_queue = Queue(0)
        self.fullframe_callback = fullframe_callback

    def new_rx_byte(self, newbyte):
        print(newbyte)
        newbyte = int.from_bytes(newbyte, byteorder='big')
        print(newbyte)
        #No frame in process
        if self.rx_state == RX_STATE.IDLE:
            if newbyte == self.SOF:
                # New frame started
                self.rx_state = RX_STATE.IN_PROCESS
                print("SOF")
        #Frame is in process        
        else:
            #Next char must be data
            if self.escape_state == ESC_STATE.NEXT:
                #Byte destuffing, this char must not be interpreted as flag
                #See serial_protocols_definition.xlsx
                self.frame_queue.put(newbyte)
                self.escape_state == ESC_STATE.IDLE
                
            #Next char can be data or flag (EOF, SOF,..)    
            elif self.escape_state == ESC_STATE.IDLE:
                if newbyte == self.EOF:
                    self.fullframe_callback(self.frame_queue)
                    print("EOF")
                    #Empty queue
                    self.frame_queue = Queue(0)
                    self.rx_state == RX_STATE.IDLE
                elif newbyte == self.ESC:
                    self.escape_state == ESC_STATE.NEXT
                else:
                    self.frame_queue.put(newbyte)
    
        

