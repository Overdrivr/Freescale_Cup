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
    def new_frame(self,rxpayload):
        frame = rxpayload
        index = 0
        command = frame[0]
        datatype = frame[1] & 0x0F

        dataid1 = frame[2]
        dataid2 = frame[3]
        dataid = dataid1 << 8 + dataid2

        #print("command [",command,"] ; datatype [", datatype,"], dataid [",dataid,"]")
        
        #Returned variable value
        if command == 0:
            if datatype == 6:
                new_values = list()
                index = 4
                while not len(frame) >= 4:
                    temp = bytearray()
                    temp.insert(1,frame[index])
                    temp.insert(1,frame[index+1])
                    temp.insert(1,frame[index+2])
                    temp.insert(1,frame[index+3])
                    index += 4

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
            index = 4
            while len(frame) - index >= 37:
                # Read datatype
                databyte = frame[index]
                datatype = databyte & 0x0F
                index+=1
                
                # Read rights
                write_rights = (databyte >> 4) == 0x0F
                
                # Read variable ID
                b1 = frame[index]
                index += 1
                b2 = frame[index]
                index += 1
                varid = b1 << 8 + b2

                # Read variable size (1 if scalar, n if array)
                array_size1 = frame[index]
                index += 1
                array_size2 = frame[index]
                index += 1
                array_size = array_size1 << 8 + array_size2
                
                # Read name
                name = ""
                    #32 characters
                for x in range(0,32):
                    c = str(chr(frame[index]))
                    name += c
                    index += 1
                
                print("New var ",name,"[",array_size,"] with id ",varid)

                #Put everything in tuple
                t = varid, datatype, array_size, write_rights, name

                #Stock the tuple in the variable list
                self.variables.append(t)
                
            #If successful, publish new table
            pub.sendMessage('logtable_update',varlist=self.variables)
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


        
