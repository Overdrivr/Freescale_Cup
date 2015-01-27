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
        self.Run_F=True
        self.grid(row=0,column=0,sticky="WENS")
        
        #Widgets
        self.bouton_emergency_shutdown = ttk.Button(self, text="STOP CAR", command = self.stop_all)
        self.bouton_emergency_shutdown.grid(column=0,row=0,sticky='NSEW',pady=3,padx=3)

        self.bouton_restart = ttk.Button(self, text="RESTART CAR", command = self.restart)
        self.bouton_restart.grid(column=1,row=0,sticky='NSEW',pady=3,padx=3)

        self.bouton_start_log = ttk.Button(self, text="START LOGGING", command = self.model.start_log)
        self.bouton_start_log.grid(column=0,row=1,sticky='NSEW',pady=3,padx=3)

        self.bouton_stop_log = ttk.Button(self, text="STOP LOGGING", command = self.model.stop_log)
        self.bouton_stop_log.grid(column=1,row=1,sticky='NSEW',pady=3,padx=3)

        # redimensionnement fenetres
        self.parent.grid_columnconfigure(0,weight=1)
        self.parent.grid_rowconfigure(0,weight=1)

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=0)
        
        
    def stop_car(self,*args):
        if(self.Run_F==True):
            try:
                self.model.write_var(0,0)
            except: 
                return
            self.Run_F=False
        else:
            try:
                self.model.write_var(0,1)
            except: 
                return
            self.Run_F=True
            
            
    def stop_record(self):
        pass

    def stop_all(self):
        self.stop_car()
        self.stop_record()

    def restart(self):
        pass
    
        
if __name__=="__main__":
    root = Tk.Tk() 
    Log_frm = Control_Frame(root,None)
    #root.minsize(width=350, height=400)

    root.mainloop()
    root.destroy()
    
