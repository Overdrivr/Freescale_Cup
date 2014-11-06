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

    def __init__(self,newframe_callback):
        self.rx_state = RX_STATE.IDLE;
        self.escape_state = ESC_STATE.IDLE;
        self.SOF = int('7f',16)
        self.EOF = int('7f',16)
        self.ESC = int('7d',16)
        self.frame_queue = Queue(0)
        self.callback = newframe_callback

    def new_rx_byte(self, newbyte):
        #No frame in process
        if self.rx_state == RX_STATE.IDLE:
            if newbyte == self.SOF:
                # New frame started
                self.rx_state = RX_STATE.IN_PROCESS
                
        #Frame is in process        
        else:
            #Next char must be data
            if self.escape_state == ESC_STATE.NEXT:
                #Byte destuffing, this char must not be interpreted as flag
                #See serial_protocols_definition.xlsx
                self.frame_queue.put(newbyte)
                self.escape_state = ESC_STATE.IDLE
                
                
            #Next char can be data or flag (EOF, SOF,..)    
            elif self.escape_state == ESC_STATE.IDLE:
                #End of frame, the payload is immediatly send to callback function
                if newbyte == self.EOF:
                    frame = self.frame_queue
                    self.callback(frame)
                    self.frame_queue = Queue(0)
                    self.rx_state = RX_STATE.IDLE
                    
                #Escaping
                elif newbyte == self.ESC:
                    self.escape_state = ESC_STATE.NEXT
                #Storing data
                else:
                    self.frame_queue.put(newbyte)

    def process_tx_payload(self, payload):
        frame = bytearray()
        frame.append(self.SOF)
        
        while not payload.empty():
            c = payload.get()
            if c == self.SOF or c == self.EOF or c == self.ESC:
                frame.append(self.ESC)
            frame.append(c)
                           
        frame.append(self.EOF)
        
        return frame
        

