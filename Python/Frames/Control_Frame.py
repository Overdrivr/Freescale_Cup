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

class Control_Frame(ttk.Frame):
    def __init__(self,parent,model,**kwargs):
        ttk.Frame.__init__(self,parent,**kwargs)
        self.parent = parent
        self.model = model
        self.connected = False
        
        self.grid(row=0,column=0,sticky="WENS")
        
        #Widgets
        self.bouton_emergency_shutdown = ttk.Button(self, text="STOP CAR", command = self.stop_all)
        self.bouton_emergency_shutdown.grid(column=0,row=0,sticky='NSEW',pady=3,padx=3)

        # redimensionnement fenetres
        self.parent.grid_columnconfigure(0,weight=1)
        self.parent.grid_rowconfigure(0,weight=1)

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=0)
        
        self.bouton_emergency_shutdown.bind("<Return>", stop_car)
        
    def stop_car(self):
        self.model.write_var(0,0)

    def stop_record(self):
        pass

    def stop_all(self):
        self.stop_car()
        self.stop_record()
        
if __name__=="__main__":
    root = Tk.Tk() 
    Log_frm = Control_Frame(root,None)
    #root.minsize(width=350, height=400)

    root.mainloop()
    root.destroy()
    
