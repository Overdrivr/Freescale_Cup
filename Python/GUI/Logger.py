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
# TODO : Use bytearray in new_frame
# TOCHECK : Empty list after update ?
# TODO : Deal with array var
# TODO : PUBLISH EVENTS FOR VALUE UPDATE
class Logger():

    def __init__(self):
        self.log_table = Queue(0)
        self.variables = list()

        pub.subscribe(self.new_frame,'new_rx_payload')

    #Process RX bytes queue
    def new_frame(self,frame):
        print("new RX frame :",frame)
        command = frame.get()
        datatype = frame.get() & 0x0F

        dataid1 = frame.get()
        dataid2 = frame.get()
        dataid = dataid1 << 8 + dataid2

        #print("command [",command,"] ; datatype [", datatype,"], dataid [",dataid,"]")
        
        #Returned variable value
        if command == 0:
            if datatype == 6:
                new_values = list()
                while not frame.qsize() >= 4:
                    temp = bytearray()
                    temp.insert(1,frame.get())
                    temp.insert(1,frame.get())
                    temp.insert(1,frame.get())
                    temp.insert(1,frame.get())

                    #Transform value to desired format
                    val = struct.unpack('i',temp)[0]

                    #Store to list
                    new_values.append(val)
                    
                #Publish the value update
                pub.sendMessage('var_value_update',dataid,new_values)
                
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
                    name_bytes = bytes(str(frame.get()),'ascii')
                    print(name_bytes)
                    name = struct.unpack('c',name_bytes)
                    print(name)
                
                print("New var ",name,"[",array_size,"] with id ",id)

                #Put everything in tuple
                t = varid, datatype, array_size, write_rights, name

                #Stock the tuple in the variable list
                self.variables.append(t)
                
            #If successful, publish new table
            pub.sendMessage('logtable_update',self.variables)
        else:
            print("Logger : unknown MCU answer : ",command)

        pass

    #Command for asking the MCU the loggable variables 
    def get_table_cmd(self):
        cmd = bytearray()
        cmd.append(int('02',16))
        cmd.append(int('07',16))
        cmd.append(int('00',16))
        return cmd

    #Command for asking the MCU to return value of specific variable 
    def get_command_read(self,var_id):
        cmd = bytearray()
        cmd.append(int('00',16))
        cmd.append(int('00',16))
        cmd.append(int('00',16))
        #TO TEST
        cmd.put(var_id >> 2)
        cmd.put(var_id & 255)
        return cmd

    def get_var_list():
        return self.variables


        
