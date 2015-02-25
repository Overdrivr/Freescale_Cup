# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from queue import Queue
import struct
from pubsub import pub

# DistantIO class : to read and write variables on the MCU from distant computer

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

        if len(frame) < 1:
            print("DistantIO error : frame size not valid")
            return None
            
        command = frame[0]
        
        # Parse 'received_variable_value' payload
        if command == 0:
            if len(frame) < 7:
                print("DistantIO error : frame size not valid")
                return None
            
            # Get dataid 
            temp = bytearray(0)
            temp.append(frame[1])
            temp.append(frame[2])
            temp.append(0)
            temp.append(0)
        
            dataid = struct.unpack("=i",temp)[0]

            # Get timepoint
            temp = bytearray(0)
            temp.append(frame[3])
            temp.append(frame[4])
            temp.append(frame[5])
            temp.append(frame[6])
        
            time = struct.unpack("=I",temp)[0]

            # Check data ID exists
            if not dataid in self.variables:
                return None
            
            new_values = list()
            index = 7

            # Decode data
            if (len(frame) - index) < self.size_lookup[self.variables[dataid]['datatype']]:
                print("DistantIO error : Unvalid frame size.")
                return
                
            while len(frame) - index >= self.size_lookup[self.variables[dataid]['datatype']]:
                # Stock raw bytes in byte array
                temp = bytearray()
                for i in range(self.size_lookup[self.variables[dataid]['datatype']]):
                    temp.append(frame[index])
                    index += 1

                # Get format
                fmt = self.type_lookup[self.variables[dataid]['datatype']]
                    
                # Transform value to desired format
                val = struct.unpack(fmt,temp)[0]
                    
                # Store to list
                new_values.append(val)

                # Combine with timepoint
                data_dict = dict()
                data_dict['time'] = time
                data_dict['values'] = new_values
                
            #Publish the value update
            try :
                pub.sendMessage('var_value_update',varid=dataid,data=data_dict)
            except:
                print("DistantIO Error : Dead listener :")
        # Parse 'received_table' payload
        elif command == 2:            
            #Empty list
            self.variables = dict()
            
            index = 1
            
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
            
        elif cmd == 'read':        
            frame.append(int('00',16))
            packed = bytes(struct.pack('=H',var_id))
            frame.extend(packed)
            
        elif cmd == 'write':
            # Check data ID exists 
            if not var_id in self.variables:
                print("DistantIO error : Data ID ",var_id," not found.")
                return None

            # Check rights
            if not self.variables[var_id]['writeable']:
                print("DistantIO error :  Variable not writeable.")
                return None

            # Find type
            fmt = self.variables[var_id]['datatype']
        
            frame.append(int('01'))

            packed = bytes(struct.pack('=H',var_id))
            frame.extend(packed)
        
            # Parse double to type
            val = None
            if fmt == 0:
                val = float(value)
                packed = bytes(struct.pack('=f',val))
            elif fmt == 1:
                val = int(value)
                packed = bytes(struct.pack('=B',val))
            elif fmt == 2:
                val = int(value)
                packed = bytes(struct.pack('=H', val))
            elif fmt == 3:
                val = int(value)
                packed = bytes(struct.pack('=I',val))
            elif fmt == 4:
                val = int(value)
                try:
                    packed = bytes(struct.pack('=b',val))
                except:
                    print("byte out of range")
                    return None
            elif fmt == 5:
                val = int(value)
                try:
                    packed = bytes(struct.pack('=h',val))
                except:
                    print("int16 out of range")
                    return None
            elif fmt == 6:
                val = int(value)
                packed = bytes(struct.pack('=i',val))              
            else:
                print("Write format ",fmt," not supported.")
                return None
            
            frame.extend(packed)
        elif cmd == 'stop':
            # Check data ID exists 
            if not var_id in self.variables:
                print("DistantIO error : Data ID ",var_id," not found.")
                return None
       
            frame.append(int('03'))

            packed = bytes(struct.pack('=H',var_id))
            frame.extend(packed)
            
        elif cmd == 'stopall':       
            frame.append(int('04'))
        else:
            return None
        
        return frame

    def get_variable_table(self):
        return self.variables

