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
        self.root = Tk.Tk()
        Tk.Frame.__init__(self,self.root,width=640, height=480)
        self.pack(fill=Tk.BOTH, expand=1)
        
        self.frame_com_ports = Tk.Frame(self)
        self.frame_com_ports.grid(column=0,row=0)
        #self.frame_com_ports.pack(side="left", fill="both",expand=True)

        self.separator = Tk.Frame(self,height=2, bg="black", bd=1, relief=Tk.RAISED)
        self.separator.grid(column=0,row=1, pady=5,sticky='EW')
            
        self.frame_logger = Tk.Frame(self)
        self.frame_logger.grid(column=0,row=2)
       # self.frame_com_ports.pack(side="left", fill="both")
        
        # Serial port thread
        self.thread_1 = SerialPortHandler()
        # Main thread 
        self.workerthread = Worker(self.thread_1)
        
        #Widgets
        self.txt_ports = Tk.Label(self.frame_com_ports,text="COM PORTS")
        self.txt_ports.grid(column=0,row=0)

        self.liste = Tk.Listbox(self.frame_com_ports,height=1)
        self.liste.grid(column=0,row=1,sticky='EW')

        self.scrollbar_liste = Tk.Scrollbar(self.liste)
        self.scrollbar_liste.config(command = self.liste.yview)
        self.liste.config(yscrollcommand = self.scrollbar_liste.set)
        self.scrollbar_liste.pack(side=Tk.RIGHT)
        
        self.bouton_refresh_ports = Tk.Button(self.frame_com_ports, text="REFRESH", command = self.read_ports)
        self.bouton_refresh_ports.grid(column=0,row=2,sticky='EW')

        self.bouton_connect = Tk.Button(self.frame_com_ports, text="CONNECT", command = self.start_com)
        self.bouton_connect.grid(column=0,row=3,sticky='EW')
        
        self.bouton_quitter = Tk.Button(self, text="x", relief=Tk.GROOVE,command = self.stop)
        self.bouton_quitter.grid(column=1,row=0,sticky='EW')

        self.txt_connected = Tk.Label(self.frame_com_ports,text="NOT CONNECTED", fg='red', width = 20)
        self.txt_connected.grid(column=0,row=4,sticky='EW')

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
    def read_ports(self):
        ports_list = self.thread_1.get_ports()
        for p, desc, hwid in sorted(ports_list):
            self.liste.insert(Tk.END,p)
        
    def start_com(self):
        self.txt_connected.config(text="CONNECTING",fg="orange")
        self.update_idletasks()
        
        if not self.liste.curselection():
            print("No port selected, aborting.")
            time.sleep(0.5)
            self.txt_connected.config(text="NOT CONNECTED",fg="red")
            self.update_idletasks()
            return
                  
        chosen_port = self.liste.get(Tk.ACTIVE)
        print("Chosen port:",chosen_port)
        self.thread_1.connect(chosen_port,115200)
        self.thread_1.start()

        self.txt_connected.config(fg = 'green')

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
        
