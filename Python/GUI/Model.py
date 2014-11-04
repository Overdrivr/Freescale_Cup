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
from Worker import SerialWorker
from Frames import *


# Top-level API for the GUI
class Model():
    def __init__(self, **kwargs):        
       
        # Serial worker thread (Serial,Protocol,Logger)
        self.workerthread = SerialWorker()
        
    def get_ports(self):
        return self.workerthread.get_ports()
        
    def start_com(self,COM_port):
        if not self.workerthread.isAlive():
            self.workerthread.start()
            
        self.workerthread.start_COM(COM_port)

    def stop_com(self,COM_port):        
        self.workerthread.stop_COM()
        print('--- COM stopped.')
         
    def stop(self):
        self.workerthread.stop()

        if self.workerthread.isAlive():
            self.workerthread.join()
        print("--- All threads stopped.")
        
    def start_logger(self):
        self.workerthread.start_logger()        
        
    def stop_logger(self):
        self.workerthread.stop_logger()  
        print('--- Logger stopped.')
        
    
        

        

        
