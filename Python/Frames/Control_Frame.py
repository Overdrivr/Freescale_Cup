# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import tkinter as Tk
import ttk as ttk
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
        
        #Widgets
        self.bouton_emergency_shutdown = ttk.Button(self, text="STOP CAR", command = self.stop_all)
        self.bouton_emergency_shutdown.grid(column=0,row=0,sticky='EW',pady=3,padx=3)

    def stop_car(self):
        self.model.write_var(0,0)

    def stop_record(self):
        pass

    def stop_all(self):
        self.stop_car()
        self.stop_record()

    
