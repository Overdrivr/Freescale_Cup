import matplotlib
matplotlib.use('TkAgg')

from numpy import arange, sin, pi
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg

import tkinter as Tk
import random
import sys
from threading import Thread
import time

from matplotlib.figure import Figure
"""
class Interface(Tk.Frame):
        
    def __init__(self, fenetre, **kwargs):
        Tk.Frame.__init__(self, fenetre, width=1, height=1, **kwargs)
        self.pack(fill=Tk.BOTH)

        #On met une figure
        self.f = Figure(figsize=(4,3), dpi=100)
        self.a = self.f.add_subplot(111)
        self.t = arange(0.0,3.0,0.01)
        self.s = sin(2*pi*self.t)
        self.a.plot(self.t,self.s)

        #plot sur canvas
        self.canvas = FigureCanvasTkAgg(self.f, master=fenetre)
        self.canvas.show()
        self.canvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)
        
        self.bouton_quitter = Tk.Button(self, text="Quitter")
        self.bouton_quitter.pack(side="left")
"""        
       
    
class SerialPortHandler(Thread):

    def __init__(self):
        Thread.__init__(self)
        self.stop_signal = 0;

    def stop(self):
        self.stop_signal = 1;

    def run(self):
        while self.stop_signal == 0:
            sys.stdout.write('A')
            sys.stdout.flush()
            attente = 0.2
            attente += random.randint(1, 60) / 100
            time.sleep(attente)


#fenetre = Tk.Tk()
#interface = Interface(fenetre)
#interface.mainloop()

            
# Thread serial port
thread_1 = SerialPortHandler()
thread_1.start()

time.sleep(5)





# Arret du thread
thread_1.stop()
thread_1.join()
