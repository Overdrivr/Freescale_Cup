import random
import sys
from threading import Thread
import time
import serial
import queue
from serial.tools.list_ports import comports
from SerialProtocol import SerialProtocol
from Logger import Logger

#Serial data processing class
class Worker(Thread):

    def __init__(self,serial_thread):
        Thread.__init__(self)
        self.stop_signal = 0
        self.serial_thread = serial_thread
        self.logger = Logger()
        self.serial_protocol = SerialProtocol(self.logger.new_frame)
        
    def stop(self):
        self.stop_signal = 1;

    def run(self):
        while self.stop_signal == 0:
            #If byte has been received
            if self.serial_thread.available():
                #Get it
                byte = self.serial_thread.read()
                #Then feed it to serial protocol
                self.serial_protocol.new_rx_byte(byte)

            


                
        
