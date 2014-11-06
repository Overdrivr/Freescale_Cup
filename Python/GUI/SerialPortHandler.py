import random
import sys
from threading import Thread
import time
import serial
from queue import Queue
from serial.tools.list_ports import comports


#Serial data processing class
class SerialPortHandler(Thread):

    def __init__(self):
        Thread.__init__(self)
        self.ser = serial.Serial()
        self.ser.timeout = 1
        self.force = False
        self.default_to = False
        self.rxqueue = Queue(0)
        self.stop_signal = 0;

    def connect(self,port,baudrate,force=False,default_to=False):
        self.ser.baudrate = baudrate
        self.ser.port = port
        self.force =force
        self.default_to = force
        
        portlist = self.get_ports()
        port_found = -1
        port_amount = 0
        terminate = False

        #List all COM ports
        print('COM ports list :')        
        for p, desc, hwid in sorted(portlist):
            print('--- %-20s %s\n' % (p, desc))
            port_amount+=1
            if p == self.ser.port:
                port_found = 1
                
        #In case no port is found 
        if port_amount == 0:
            print('No COM port found.')
            print('   - can use \'force\' mode to try connect anyway.')
            terminate = True
            
            if self.force:
                  terminate = False
                  
        #In case ports are found but not chosen one
        if port_amount > 0 and port_found == -1:
            print(port,' port not found.')
            print('   - can use \'default\' mode to default to fall back to a valid port.')
            terminate = True
            
            if self.default_to:
                terminate = False
                ser.port = [x[1] for x in portlist][0]
                
        #Exit prematurely if error
        if terminate:
            print(port, 'port non valid, aborting.')
            return

        self.ser.open()
        print('Connected to port ',self.ser.port)
        return

        
    def get_ports(self):
        return serial.tools.list_ports.comports()
        
    def stop(self):
        self.stop_signal = 1;

    #Returns amount of available bytes for reading
    def available(self):
        return self.rxqueue.qsize() > 0

    #Returns first available byte for reading
    def read(self):
        return self.rxqueue.get()

    def write(self, frame):
        print("written :",frame)
        return self.ser.write(frame)

    def run(self):
        
        #Main serial loop      
        while self.stop_signal == 0:
            if self.ser.inWaiting() > 0:
                serialout = self.ser.read()
                self.rxqueue.put(serialout)

        #Exit
        self.ser.close()
        
