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
from Model import Model

class Application(Tk.Frame):
        
    def __init__(self, **kwargs):
        #Create window
        self.root = Tk.Tk()

        #Init master frame
        Tk.Frame.__init__(self,self.root,width=640, height=480)
        self.pack(fill=Tk.BOTH, expand=1)

        #Create Model
        self.model = Model()

        #COM Frame
        self.frame_com_ports = COM_Frame(self,self.model)
        self.frame_com_ports.grid(column=0,row=0)

        #Separator
        self.separator = Tk.Frame(self,height=2, bg="black", bd=1, relief=Tk.RAISED)
        self.separator.grid(column=0,row=1, pady=5,sticky='EW')

        #Logger frame
        self.frame_logger = Tk.Frame(self)
        self.frame_logger.grid(column=0,row=2)
        
        #Widgets
        self.bouton_quitter = Tk.Button(self, text="x", relief=Tk.GROOVE,command = self.stop)
        self.bouton_quitter.grid(column=1,row=0,sticky='EW')

        self.txt_log = Tk.Label(self.frame_logger,text="LOGGER")
        self.txt_log.grid(column=0,row=6,sticky='EW')
        
        """self.f = Figure(figsize=(4,3), dpi=100)
        self.a = self.f.add_subplot(111)
        self.t = arange(0.0,3.0,0.01)
        self.s = sin(2*pi*self.t)
        self.a.plot(self.t,self.s)

        #plot sur canvas
        self.canvas = FigureCanvasTkAgg(self.f, master=fenetre)
        self.canvas.show()
        self.canvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)
        """          


    def stop(self):
        self.model.stop()
        
