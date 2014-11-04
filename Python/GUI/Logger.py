import random
import sys
from threading import Thread
import time
from queue import Queue
import struct

# Logger class
# TODO : use is_logger_started
# TODO : add variable table
# TODO : parse all datatype
# TODO : ...?

class Logger():

    def __init__(self):
        self.log_table = Queue(0)
        self.variables = list()
        self.is_logger_started = 0

    #Process RX bytes queue
    def new_frame(self,frame):
        command = int.from_bytes(frame.get(),"big")
        datatype = int.from_bytes(frame.get(),"big")

        dataid1 = int.from_bytes(frame.get(),"big")
        dataid2 = int.from_bytes(frame.get(),"big")
        dataid = dataid1 << 8 + dataid2

        #print("command [",command,"] ; datatype [", datatype,"], dataid [",dataid,"]")
        
        #Returned variable value
        if command == 0:
            if datatype == 6:
                #while not frame.empty():
                    temp = bytearray()
                    temp.insert(1,int.from_bytes(frame.get(),"big"))
                    temp.insert(1,int.from_bytes(frame.get(),"big"))
                    temp.insert(1,int.from_bytes(frame.get(),"big"))
                    temp.insert(1,int.from_bytes(frame.get(),"big"))

                    #Transform value to desired format
                    val = struct.unpack('i',temp)[0]

                    #Set new value to table
                    print("val = ",val)
                
        #Returned variable table
        elif command == 2:
            while not frame.empty():
                b1 = frame.get()
                b2 = frame.get()
                id = b1 << 8 + b2
                #Read 32 characters

                #If successful, update logger state
                self.is_logger_started = 1
        else:
            print("Logger : unknown MCU answer")

        pass

    #Command for asking the MCU the loggable variables 
    def get_table_cmd(self):
        cmd = Queue(3)
        cmd.put(bytes(0x02))
        cmd.put(bytes(0x07))
        cmd.put(bytes(0x00))
        return cmd

    def is_started(self):
        return self.is_logger_started

    def stop(self):
        self.is_logger_started = 0
        
    #Command for asking the MCU to return value of specific variable 
    def get_command_read(self,var_id):
        cmd = Queue(5)
        cmd.put(bytes(0x00))
        cmd.put(bytes(0x00))
        cmd.put(bytes(0x00))
        cmd.put(bytes(var_id >> 2))
        cmd.put(bytes(var_id & 0x00FF))
        return cmd


        
