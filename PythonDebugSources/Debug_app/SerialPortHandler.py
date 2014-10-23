import random
import sys
from threading import Thread
import time
import serial
from queue import Queue
from serial.tools.list_ports import comports


#Serial data processing class
class SerialPortHandler(Thread):

    def __init__(self,port,baudrate,force=False,default=False):
        Thread.__init__(self)
        self.ser = serial.Serial()
        self.ser.baudrate = baudrate
        self.ser.port = port
        self.ser.timeout = 1
        self.force = force
        self.default = default
        self.rxqueue = Queue(0)
        self.stop_signal = 0;

    def stop(self):
        self.stop_signal = 1;
        #TODO : Also force stop serial port here

    #Returns amount of available bytes for reading
    def available(self):
        return self.rxqueue.qsize()

    #Returns first available byte for reading
    def read(self):
        return self.rxqueue.get()

    def run(self):
        portlist = serial.tools.list_ports.comports()
        port_found = -1
        port_amount = 0
        terminate = False

        #List all COM ports
        print('COM ports list :')        
        for port, desc, hwid in sorted(portlist):
            sys.stderr.write('--- %-20s %s\n' % (port, desc))
            port_amount+=1
            if port == self.ser.port:
                port_found = port
                
        #In case no port is found 
        if port_amount == 0:
            print('No COM port found.')
            print('   - can use \'force\' mode to try connect anyway.')
            terminate = True
            
            if self.force:
                  terminate = False
                  
        #In case ports are found but not chosen one
        if port_amount > 0 and port_found == -1:
            print('COM port not found.')
            print('   - can use \'default\' mode to default to fall back to a valid port.')
            
            if self.default:
                terminate = False
                ser.port = [x[1] for x in portlist][0]
                
        #Exit prematurely if error
        if terminate:
            print('exiting.')
            return

                    
        self.ser.open()
        #self.ser.isOpen()
        print('Connected to port ',port_found)
        
        #Main serial loop      
        while self.stop_signal == 0:
            serialout = ""
            serialout = self.ser.read()
            if serialout != "":
                self.rxqueue.put(serialout)

        #Exit
        self.ser.close()
        
