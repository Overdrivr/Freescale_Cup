# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from enum import Enum
from pubsub import pub

class RX_STATE(Enum):
    IDLE = 0
    IN_PROCESS = 1
    
class ESC_STATE(Enum):
    IDLE = 0
    NEXT = 1

#Robust serial protocol with bit stuffing
# ON RX ERROR, RESET PROTOCOL ?
class SerialProtocol():

    def __init__(self):
        self.rx_state = RX_STATE.IDLE;
        self.escape_state = ESC_STATE.IDLE;
        self.SOF = int('f7',16)
        self.EOF = int('7f',16)
        self.ESC = int('7d',16)
        self.payload = bytearray() 
        pub.subscribe(self.new_rx_byte,"new_rx_byte")

    def new_rx_byte(self, rxbyte):
        newbyte = int.from_bytes(rxbyte,byteorder='big')
        #No frame in process
        if self.rx_state == RX_STATE.IDLE:
            if newbyte == self.SOF:
                # New frame started
                self.rx_state = RX_STATE.IN_PROCESS
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
                
                
            #Next char can be data or flag (EOF, SOF,..)    
            elif self.escape_state == ESC_STATE.IDLE:
                #End of frame, the payload is immediatly send to callback function
                if newbyte == self.EOF:
                    pub.sendMessage("new_rx_payload",rxpayload=self.payload)
                    self.payload = bytearray()
                    self.rx_state = RX_STATE.IDLE
                    
                #Escaping
                elif newbyte == self.ESC:
                    self.escape_state = ESC_STATE.NEXT
                #Storing data
                else:
                    self.payload.append(newbyte)

    def process_tx_payload(self, rxpayload):
        frame = bytearray()
        frame.append(self.SOF)
        
        for c in rxpayload:
            if c == self.SOF or c == self.EOF or c == self.ESC:
                frame.append(self.ESC)
            frame.append(c)
                           
        frame.append(self.EOF)
        
        return frame
        

