# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from enum import Enum
from pubsub import pub
from queue import Queue

class RX_STATE(Enum):
    IDLE = 0
    IN_PROCESS = 1
    
class ESC_STATE(Enum):
    IDLE = 0
    NEXT = 1

#Robust serial protocol with bit stuffing
class Protocol():

    def __init__(self):
        self.rx_state = RX_STATE.IDLE;
        self.escape_state = ESC_STATE.IDLE;
        self.SOF = int('f7',16)
        self.EOF = int('7f',16)
        self.ESC = int('7d',16)
        self.payload = bytearray()
        # Max amount of payloads
        self.payloads = Queue(10000)#TODO : CHECK BEHAVIOR
        self.framesize = 0;
        self.processed_octets = 0

    def process_rx(self, rxbyte):
        newbyte = int.from_bytes(rxbyte,byteorder='big')
        self.processed_octets += 1
        
        #No frame in process
        if self.rx_state == RX_STATE.IDLE:
            if newbyte == self.SOF:
                # New frame started
                self.rx_state = RX_STATE.IN_PROCESS
                self.framesize = 0;
            else:
                t = newbyte,
                pub.sendMessage('new_ignored_rx_byte',rxbyte=bytes(t))
                
        #Frame is in process        
        else:
            #Next char must be data
            if self.escape_state == ESC_STATE.NEXT:
                #Byte destuffing, this char must not be interpreted as flag
                #See serial_protocols_definition.xlsx
                self.payload.append(newbyte)
                self.escape_state = ESC_STATE.IDLE
                self.framesize += 1;
                
            #Next char can be data or flag (EOF, SOF,..)    
            elif self.escape_state == ESC_STATE.IDLE:
                #End of frame, the payload is immediatly send to callback function
                if newbyte == self.EOF:
                    #pub.sendMessage("new_rx_payload",rxpayload=self.payload)
                    self.payloads.put(self.payload)
                    self.payload = bytearray()
                    self.rx_state = RX_STATE.IDLE
                    
                #Receive a SOF while a frame is running, error
                elif newbyte == self.SOF:
                    print("Protocol : Received frame unvalid, discarding.", self.payload)
                    self.payload = bytearray()
                    self.rx_state = RX_STATE.IDLE
                                        
                #Escaping
                elif newbyte == self.ESC:
                    self.escape_state = ESC_STATE.NEXT
                
                #Storing data
                else:
                    self.payload.append(newbyte)
                    self.framesize += 1;

    def available(self):
        return not self.payloads.empty()

    def get(self):
        if self.payloads.empty():
            return None
        else:
            return self.payloads.get()
        
    def process_tx(self, rxpayload):
        frame = bytearray()
        frame.append(self.SOF)
        
        for c in rxpayload:
            if c == self.SOF or c == self.EOF or c == self.ESC:
                frame.append(self.ESC)
            frame.append(c)
                           
        frame.append(self.EOF)
        
        return frame

    def get_processed_octets(self):
        return self.processed_octets
        

