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

class Application(Tk.Frame):
        
    def __init__(self, **kwargs):
        self.fenetre = Tk.Tk()
        Tk.Frame.__init__(self, self.fenetre, width=1, height=1, **kwargs)
        self.pack(fill=Tk.BOTH)

        # Serial port thread
        self.thread_1 = SerialPortHandler()
        # Main thread 
        self.workerthread = Worker(self.thread_1)

        self.grid()
        
        #Widgets
        self.txt_ports = Tk.Label(self,text="COM Ports")
        self.txt_ports.grid(column=0,row=0)

        self.liste = Tk.Listbox(self,height=2)
        self.liste.grid(column=0,row=1,sticky='EW')

        self.bouton_refresh_ports = Tk.Button(self, text="Refresh ports", command = self.read_ports)
        self.bouton_refresh_ports.grid(column=0,row=2,sticky='EW')

        self.bouton_connect = Tk.Button(self, text="Connect", command = self.start_com)
        self.bouton_connect.grid(column=0,row=3,sticky='EW')
        
        self.bouton_quitter = Tk.Button(self, text="x", relief=Tk.GROOVE,command = self.stop)
        self.bouton_quitter.grid(column=1,row=0,sticky='EW')

                
        
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
    def read_ports(self):
        ports_list = self.thread_1.get_ports()
        for p, desc, hwid in sorted(ports_list):
            self.liste.insert(Tk.END,p)
        
    def start_com(self):
        if not self.liste.curselection():
            print("No port selected, aborting.")
            return
                  
        chosen_port = self.liste.get(Tk.ACTIVE)
        print("Chosen port:",chosen_port)
        self.thread_1.connect(chosen_port,115200)
        self.thread_1.start()

        self.workerthread.start()
        self.workerthread.get_MCU_table()

    def stop(self):
        #Stop threads
        print("--- Stopping threads...")
        self.workerthread.stop()
        self.thread_1.stop()

        if self.workerthread.isAlive():
            self.workerthread.join()
        if self.thread_1.isAlive():
            self.thread_1.join()

        print('--- Threads stopped.')
        
