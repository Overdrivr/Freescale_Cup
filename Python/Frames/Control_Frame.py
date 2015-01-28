# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import tkinter as Tk
import tkinter.ttk as ttk
from pubsub import pub
import numpy as np
from collections import deque

"""
Control GUI Frame
"""

class Control_Frame(ttk.LabelFrame):
    def __init__(self,parent,model,**kwargs):
        ttk.LabelFrame.__init__(self,parent,text="CarControl",**kwargs)
        self.parent = parent
        self.model = model
        self.connected = False
        self.grid(row=0,column=0,sticky="WENS")
        
        #Widgets
        # Row 0
        self.bouton_emergency_shutdown = ttk.Button(self, text="STOP CAR", command = self.stop_all)
        self.bouton_emergency_shutdown.grid(column=0,row=0,sticky='NSEW',pady=3,padx=3)

        self.bouton_restart = ttk.Button(self, text="RESTART CAR", command = self.restart)
        self.bouton_restart.grid(column=1,row=0,sticky='NSEW',pady=3,padx=3)

        self.speed_label = ttk.Label(self,text="Speed:")
        self.speed_label.grid(column=2,row=0,sticky="NSEW",padx=3,pady=3)

        self.speed = Tk.DoubleVar()
        self.speed.set(1.0)

        self.speed_entry = ttk.Entry(self,textvariable=self.speed,width=4)
        self.speed_entry.grid(column=3,row=0,sticky="NSEW",padx=3,pady=3)
        # Row 1
        
        self.bouton_start_log = ttk.Button(self, text="START LOGGING", command = self.model.start_log)
        self.bouton_start_log.grid(column=0,row=1,sticky='NSEW',pady=3,padx=3)

        self.bouton_stop_log = ttk.Button(self, text="STOP LOGGING", command = self.model.stop_log)
        self.bouton_stop_log.grid(column=1,row=1,sticky='NSEW',pady=3,padx=3)

        self.queuesize_label = ttk.Label(self,text="Queue Size:")
        self.queuesize_label.grid(column=2,row=1,sticky="NSEW",padx=3,pady=3)

        self.queuesize = Tk.DoubleVar()
        self.queuesize.set(3000.0)

        self.queuesize_entry = ttk.Entry(self,textvariable=self.queuesize,width=6)
        self.queuesize_entry.grid(column=3,row=1,sticky="NSEW",padx=3,pady=3)

        # redimensionnement fenetres
        self.parent.grid_columnconfigure(0,weight=1)
        self.parent.grid_rowconfigure(0,weight=1)

        #self.grid_columnconfigure(0, weight=1)
        #self.grid_rowconfigure(0, weight=0)
        
        
    def stop_car(self,*args):
        self.model.write_var(0,0)
            
    def stop_record(self):
        pass

    def stop_all(self):
        self.stop_car()
        self.stop_record()

    def restart(self):
        self.model.write_var(0,self.speed.get())
    
        
if __name__=="__main__":
    class dummyModel():
        def __init__(self):
            pass
        def start_log():
            pass
        def stop_log():
            pass
    root = Tk.Tk()
    model = dummyModel()
    Log_frm = Control_Frame(root,model)
    root.mainloop()
    
