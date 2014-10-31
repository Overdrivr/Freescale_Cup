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

        #Start threads
        # Serial port thread
        """thread_1 = SerialPortHandler('COM8',115200)
        thread_1.start()

        # Main thread 
        workerthread = Worker(thread_1)
        workerthread.start()
        workerthread.get_MCU_table()"""
        
        #Widgets
        self.bouton_quitter = Tk.Button(self, text="Quitter", command = self.stop)
        self.bouton_quitter.pack(side="left")
        """
        liste = Tk.Listbox(self)
        liste.insert(Tk.END,"COM5")
        liste.insert(Tk.END,"COM6")
        liste.pack()
        """
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
        #Stop threads
        print("***************Stopping threads*************")
        """workerthread.stop()
        thread_1.stop()

        workerthread.join()
        thread_1.join()

        print('done')
        """
