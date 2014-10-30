import random
import sys
from threading import Thread
import time
from queue import Queue

#Serial data processing class
class Logger():

    def __init__(self):
        self.log_table = Queue(0)
        self.variables = list()

    #Process RX bytes queue
    def new_frame(self,frame):
        command = frame.get()
        datatype = frame.get()
        dataid1 = frame.get()
        dataid2 = frame.get()
        dataid = int.from_bytes(dataid1,"big") << 8 + int.from_bytes(dataid2,"big")

        print("command [",command,"] ; datatype [", datatype,"], dataid [",dataid"]")
        
        #Returned variable value
        if current_byte == bytes(0x00):
            
        #Returned variable table
        elif current_byte == bytes(0x02):
            while not frame.empty():
                b1 = frame.get()
                b2 = frame.get()
                id = b1 << 8 + b2
                #Read 32 characters
        else
            print("Logger : unknown MCU answer")

        pass

    #Command for asking the MCU the loggable variables 
    def get_command_read_variable_table(self):
        cmd = Queue(3)
        cmd.put(bytes(0x02))
        cmd.put(bytes(0x07))
        cmd.put(bytes(0x00))
        return cmd
        
    #Command for asking the MCU to return value of specific variable 
    def get_command_read_variable(self,var_id):
        cmd = Queue(5)
        cmd.put(bytes(0x00))
        cmd.put(bytes(0x00))
        cmd.put(bytes(0x00))
        cmd.put(bytes(var_id >> 2))
        cmd.put(bytes(var_id & 0x00FF))
        return cmd


        
