# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from Model import Model
from pubsub import pub
from SerialPort import SerialPort
from threading import Timer
import struct as struct
import time

def stop():
    print("stop")
    port.stop() 
    port.disconnect()
      

def printout():
    end = time.time()
    print(end - start,"s -------------------------")
    print("chars received : ",count)
    print("in waiting :",port.get_rxloadmax())
    print("errors :",errorcount)
    
    if port.isAlive():
        t2 = Timer(1.0, printout)
        t2.start()
    
port = SerialPort()
port.start()
port.connect("COM5",115200)
synced = False
count = 0
errorcount = 0
index = 0

sequence = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

t = Timer(10.0, stop)
t.start()

start = time.time()
t2 = Timer(1.0, printout)
t2.start()

while port.isAlive():
    if port.char_available():
        ch = port.get_char()
        c = struct.unpack('=B',ch)[0]
        if not c is None :
            # Sync with sequence if not synced
            if not synced :
                if c == sequence[0]:
                    synced = True
                    index = 1
            # Look for errors
            else:
                if c == sequence[index]:
                    index = (index + 1) % 10
                    count += 1
                else:
                    errorcount += 1
                    synced = False

                           
port.join()
print("Done.")
