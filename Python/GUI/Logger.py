import random
import sys
from threading import Thread
import time
from queue import Queue
import struct
from pubsub import pub

# Logger class
# TODO : parse all datatype
# TODO : Deal with array var

class Logger():

    def __init__(self):
        self.log_table = Queue(0)
        self.variables = list()
        self.type_lookup = {0 : '=f',
                            6 : 'i'
                            }
        pub.subscribe(self.new_frame,'new_rx_payload')

    #Process RX bytes queue
    def new_frame(self,rxpayload):
        frame = rxpayload
        index = 0
        command = frame[0]
        datatype = frame[1] & 0x0F

        temp2 = bytearray()
        temp2.insert(1,frame[2])
        temp2.insert(1,frame[3])
        temp2.insert(1,0)
        temp2.insert(1,0)
        dataid = struct.unpack("i",temp2)[0]
        
        # Parse 'received_variable_value' payload

        
        if command == 0:
            #int
            if datatype == 6:
                new_values = list()
                index = 4
                
                if (len(frame) - 4) < 4:
                    print("Unvalid frame size")
                    return
                
                while len(frame) - index >= 4:
                    # Stock raw bytes in byte array
                    temp = bytearray()
                    temp.insert(1,frame[index])
                    temp.insert(1,frame[index+1])
                    temp.insert(1,frame[index+2])
                    temp.insert(1,frame[index+3])
                    index += 4
                   
                    # Get format
                    fmt = self.type_lookup[datatype]
                    
                    # Transform value to desired format
                    val = struct.unpack(fmt,temp)[0]
                    
                    # Store to list
                    new_values.append(val)
                    
                #Publish the value update
                pub.sendMessage('var_value_update',varid=dataid,value_list=new_values)

                
            #float
            elif datatype == 0:
                new_values = list()
                index = 4
                
                if (len(frame) - 4) < 4:
                    print("Unvalid frame size")
                    return
                
                while len(frame) - index >= 4:
                    # Stock raw bytes in byte array
                    temp = bytearray()
                    temp.append(frame[index])
                    temp.append(frame[index+1])
                    temp.append(frame[index+2])
                    temp.append(frame[index+3])
                    index += 4

                    # Get format
                    fmt = self.type_lookup[datatype]
                    
                    # Transform value to desired format
                    val = struct.unpack(fmt,temp)[0]
                    
                    # Store to list
                    new_values.append(val)
                print("Lenght : ",len(new_values))
                #Publish the value update
                pub.sendMessage('var_value_update',varid=dataid,value_list=new_values)

        # Parse 'received_table' payload
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
                temp = bytearray()
                temp.append(frame[index])
                temp.append(frame[index+1])
                index += 2
                
                varid = struct.unpack('H',temp)[0]

                # Read variable size (1 if scalar, n if array)
                temp = bytearray()
                temp.append(frame[index])
                temp.append(frame[index+1])
                index += 2
                
                array_size = struct.unpack('H',temp)[0]
                
                # Read name
                name = ""
                
                #32 characters max
                for x in range(0,32):
                    c = str(chr(frame[index]))
                    name += c
                    index += 1
                
                print("New var ",name,"[",array_size,"] with id ",varid, 'of type ',datatype, 'with rights ',write_rights)

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
    def get_read_cmd(self,var_id):
        cmd = bytearray()
        cmd.append(int('00',16))
        cmd.append(int('00',16))
        #TO TEST FOR IDs > 255
        cmd.append(var_id)
        cmd.append((var_id >> 8))
        return cmd

    def get_var_list():
        return self.variables

    


        
