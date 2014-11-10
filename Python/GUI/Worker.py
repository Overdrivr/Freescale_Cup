import random
import sys
from threading import Thread
import time
import queue

from SerialProtocol import SerialProtocol
from SerialPortHandler import SerialPortHandler
from Logger import Logger

# Reads serial port in a thread
# and feeds it to serial protocol for processing

# TODO : Print unprocessed serial com

# TODO : With publisher system, this thread becomes useless. MOVE CODE TO MODEL AND REMOVE 
class SerialWorker(Thread):

    def __init__(self):
        Thread.__init__(self)
        self.stop_signal = 0
        self.is_logger_on = 0

        # Serial port thread
        self.serialthread = SerialPortHandler()

        # Logger
        self.logger = Logger()

        # Init serial protocol
        self.serial_protocol = SerialProtocol()

    def get_ports(self):
        ports_list = self.serialthread.get_ports()
        return ports_list

    def start_COM(self,COM_port):
        # Connect
        self.serialthread.connect(COM_port,115200)
        # Start monitoring thread 
        self.serialthread.start()

    def stop_COM(self):
        
        self.stop_logger()
        self.serialthread.stop()

        if self.serialthread.isAlive():
            self.serialthread.join()
        
    def start_logger(self):
        #Get command for querying variable table MCU side
        cmd = self.logger.get_table_cmd()
        #Feed command to serial protocol payload processor
        frame = self.serial_protocol.process_tx_payload(cmd)        
        #Send command
        self.serialthread.write(frame)
        
    def stop_logger(self):
        #Tell the MCU to stop sending data
        pass
        
    def stop(self):
        self.stop_signal = 1;
        self.stop_COM()

    def run(self):
        while self.stop_signal == 0:
            #If byte has been received
            if self.serialthread.available():
                #Get it
                byte = int.from_bytes(self.serialthread.read(),'big')
                print(byte)
                #Then feed it to serial protocol
                self.serial_protocol.new_rx_byte(byte)

            


                
        
