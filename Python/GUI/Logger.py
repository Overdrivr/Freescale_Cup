import random
import sys
from threading import Thread
import time
from queue import Queue
import struct
from pubsub import pub

# Logger class
# TODO : parse all datatype
# TODO : add variable size in tuple

class Logger():

    def __init__(self):
        self.log_table = Queue(0)
        self.variables = list()

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
            #Empty list
            self.variables = list()
            
            while frame.qsize() >= 37:
                # Read datatype
                databyte = frame.get()
                datatype = databyte & 0x0F
                
                # Read rights
                write_rights = (databyte >> 4) == 0x0F
                
                # Read variable ID
                b1 = frame.get()
                b2 = frame.get()
                varid = b1 << 8 + b2

                # Read variable size (1 if scalar, n if array)
                array_size1 = frame.get()
                array_size2 = frame.get()
                array_size = array_size1 << 8 + array_size2
                
                # Read name
                name = ""
                    #32 characters
                for x in range(0,32):
                    c = frame.get()
                    #TODO : TO CHECK
                    name += struct.unpack('c',c)
                
                print("New var ",name,"[",arraysize,"] with id ",id)

                #Put everything in tuple
                t = varid, datatype, arraysize, write_rights, name

                #Stock the tuple in the variable list
                self.variables.append(t)
                
            #If successful, publish new table
            pub.sendMessage('logtable_received',self.variables)
        else:
            print("Logger : unknown MCU answer")

        pass

    #Command for asking the MCU the loggable variables 
    def get_table_cmd(self):
        cmd = Queue(0)
        cmd.put(int('02',16))
        cmd.put(int('07',16))
        cmd.put(int('00',16))
        return cmd

    #Command for asking the MCU to return value of specific variable 
    def get_command_read(self,var_id):
        cmd = Queue(0)
        cmd.put(int('00',16))
        cmd.put(int('00',16))
        cmd.put(int('00',16))
        #TO TEST
        cmd.put(var_id >> 2)
        cmd.put(var_id & 255)
        return cmd

    def get_var_list():
        return self.variables


        
