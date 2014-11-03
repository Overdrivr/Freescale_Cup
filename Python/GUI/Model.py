#import matplotlib
#matplotlib.use('TkAgg')

#from numpy import arange, sin, pi
#from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg

import tkinter as Tk
import random
import sys
from threading import Thread
import time

#from matplotlib.figure import Figure
from SerialPortHandler import SerialPortHandler
from Worker import Worker
from Frames import *

class Model():
    def __init__(self, **kwargs):        
        # Serial port thread
        self.serialthread = SerialPortHandler()
        # Main thread 
        self.workerthread = Worker(self.serialthread)
        
    def get_ports(self):
        ports_list = self.serialthread.get_ports()
        return ports_list
        
    def start_com(self,COM_port):        
        print("Chosen port:",COM_port)
        self.serialthread.connect(COM_port,115200)
        self.serialthread.start()

        self.workerthread.start()        
        self.workerthread.get_MCU_table()

        return True

    def stop(self):
        #Stop threads
        print("--- Stopping threads...")
        self.workerthread.stop()
        self.serialthread.stop()

        if self.workerthread.isAlive():
            self.workerthread.join()
        if self.serialthread.isAlive():
            self.serialthread.join()

        print('--- Threads stopped.')
