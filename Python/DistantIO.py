# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from queue import Queue
import struct
from pubsub import pub

# DistantIO class : to read and write variables on the MCU from distant computer
# TODO : parse all datatype

class DistantIO():

    def __init__(self):
        """
        Dictionnary holding the variable table
        """
        # [variable_id]['datatype']   : data type (float, int32, uint8, etc.)
        # [variable_id]['octets']     : data size in octets
        # [variable_id]['is_array']   : True or False
        # [variable_id']['amount']    : 1 if scalar, array size if array
        # [variable_id]['writeable']  : if the variable can be written
        # [variable_id]['name']       : variable name
        self.variables = dict()

        # For converting dataid to a type
        self.type_lookup = {0 : '=f',
                            1 : '=B',
                            2 : '=H',
                            3 : '=I',
                            4 : '=b',
                            5 : '=h',
                            6 : '=i'
                            }
        self.size_lookup = {0 : 4,
                            1 : 1,
                            2 : 2,
                            3 : 4,
                            4 : 1,
                            5 : 2,
                            6 : 4}

    #Process RX bytes queue
    def decode(self,rxpayload):
        frame = rxpayload
        index = 0

        if len(frame) < 4:
            print("DistantIO error : frame size not valid")
            return
            
        command = frame[0]
        datatype = frame[1] & 0x0F

        temp2 = bytearray(0)
        temp2.append(frame[2])
        temp2.append(frame[3])
        temp2.append(0)
        temp2.append(0)
        
        dataid = struct.unpack("=i",temp2)[0]
        
        # Parse 'received_variable_value' payload
        if command == 0:

            # Check data ID exists
            if not dataid in self.variables:
                #print("DistantIO error : Data ID ",dataid," not found.")
                return

            # Check datatype exists
            if not datatype in self.type_lookup:
                print("DistantIO error : Datatype ",datatype," unknown.")
                return

            if not datatype in self.size_lookup:
                print("DistantIO error : Datatype ",datatype," size unknown.")
                return

            # Check variable datatype matches
            if not datatype == self.variables[dataid]['datatype']:
                print("DistantIO error : Matching error between table and RX.")
                return
            
            new_values = list()
            index = 4

            # Decode data
            if (len(frame) - index) < self.size_lookup[datatype]:
                print("DistantIO error : Unvalid frame size.")
                return
                
            while len(frame) - index >= self.size_lookup[datatype]:
                # Stock raw bytes in byte array
                temp = bytearray()
                for i in range(self.size_lookup[datatype]):
                    temp.append(frame[index])
                    index += 1

                # Get format
                fmt = self.type_lookup[datatype]
                    
                # Transform value to desired format
                val = struct.unpack(fmt,temp)[0]
                    
                # Store to list
                new_values.append(val)
                
            #Publish the value update
            pub.sendMessage('var_value_update',varid=dataid,value_list=new_values)
             
        # Parse 'received_table' payload
        elif command == 2:
            #Empty list
            self.variables = dict()
            index = 4
            while len(frame) - index >= 37:
                # Read datatype
                databyte = frame[index]
                datatype = databyte & 0x0F
                index+=1
                
                # Read rights
                writeable = (databyte >> 4) == 0x0F
                
                # Read variable ID
                temp = bytearray()
                temp.append(frame[index])
                temp.append(frame[index+1])
                index += 2
                
                varid = struct.unpack('H',temp)[0]

                # Read variable size in octets
                temp = bytearray()
                temp.append(frame[index])
                temp.append(frame[index+1])
                index += 2
                
                octets = struct.unpack('H',temp)[0]

                # Compute array size if array
                if octets == self.size_lookup[datatype]:
                    is_array = True
                    array_size = 1
                else:
                    is_array = False
                    array_size = octets / self.size_lookup[datatype]
                    if not (octets % self.size_lookup[datatype]) == 0:
                        print("DistantIO error : Stride not correct, computed array size is wrong.")
                    
                
                # Read name
                name = ""
                
                #32 characters max
                for x in range(0,32):
                    c = str(chr(frame[index]))
                    name += c
                    index += 1

                # Stock everything in dictionnary
                self.variables[varid] = dict()
                
                self.variables[varid]['datatype'] = datatype
                self.variables[varid]['octets'] = octets
                self.variables[varid]['is_array'] = is_array
                self.variables[varid]['amount'] = array_size
                self.variables[varid]['writeable'] = writeable
                self.variables[varid]['name'] = name
                
            #If successful, publish new table
            pub.sendMessage('logtable_update',varlist=self.variables)
        else:
            print("DistantIO error : unknown MCU answer : ",frame)

        pass

    #Command for asking the MCU the loggable variables 
    def encode(self, cmd, var_id=0 ,value=0): 
        frame = bytearray()
    
        if cmd == 'table':
            frame.append(int('02',16))
            frame.append(int('07',16))
            frame.append(int('00',16))
            
        elif cmd == 'read':        
            frame = bytearray()
            frame.append(int('00',16))
            frame.append(int('00',16))#IGNORED ?
            packed = bytes(struct.pack('=H',var_id))
            frame.extend(packed)
            
        elif cmd == 'write':
            # Check data ID exists 
            if not var_id in self.variables:
                print("DistantIO error : Data ID ",dataid," not found.")
                return

            # Check rights
            if not self.variables[var_id]['writeable']:
                print("DistantIO error :  Variable not writeable.")
                return

            # Find type
            fmt = self.variables[var_id]['datatype']
        
            frame.append(int('01'))
            frame.append(int(fmt))

            packed = bytes(struct.pack('=H',var_id))
            frame.extend(packed)
        
            # Parse double to type
            val = None
            if fmt == 0:
                val = float(value)
                packed = bytes(struct.pack('=f',val))
            elif fmt == 6:
                val = int(value)
                packed = bytes(struct.pack('=i',val))
            else:
                print("Write format not supported.")
                return
            
            frame.extend(packed)       
        else:
            return
        
        return frame

    def get_variable_table(self):
        return self.variables

