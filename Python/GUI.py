# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import tkinter as Tk
import tkinter.ttk as ttk
from Model import Model
from threading import Timer
from pubsub import pub
from array import array
from Frames.COM_Frame import *
from Frames.Logger_Frame import *
from Frames.Control_Frame import *
from Frames.FileExplorer_Frame import *
from DataLogger import *

class Application(ttk.Frame):
        
    def __init__(self,parent,**kwargs):
        # Init
        self.parent = parent
        ttk.Frame.__init__(self,parent,**kwargs)
        # Init configuration
        ttk.Style().configure("BW.TLabel")
        ttk.Style().configure("BW.TButton")
        
        self.grid(row=0,column=0,sticky="WENS")

        # Create Model
        self.model = Model()

        # Init notebook holding the tabs
        self.tabs = ttk.Notebook(self)

        # Init tabs
        self.measurement_tab = ttk.Frame(self)
        self.file_explorer_tab = FileExplorer_Frame(self,self.model)

        ## MEASUREMENT TAB ITEMS
        ### COM Frame
        self.frame_com_ports = COM_Frame(self.measurement_tab,self.model,relief=Tk.GROOVE)
        self.frame_com_ports.grid(column=0,row=0,sticky='NSEW',pady=2,padx=5)

        ### Logger frame
        self.frame_logger = Logger_Frame(self.measurement_tab,self.model,relief=Tk.GROOVE)
        self.frame_logger.grid(column=0,row=1,sticky='NSEW',pady=2,padx=5)

        ### Control frame
        self.frame_ctrl = Control_Frame(self.measurement_tab,self.model,relief=Tk.GROOVE)
        self.frame_ctrl.grid(column=0,row=2,sticky='NSEW',pady=2,padx=5)

    
        ## FILE EXPLORER TAB ITEM

        # Add frames to notebook
        self.tabs.add(self.measurement_tab,text='LiveFeed')
        self.tabs.add(self.file_explorer_tab,text='Measurements')
        self.tabs.grid(column=0,row=0,sticky='WENS')

        # Quit button
        self.bouton_quitter = ttk.Button(self, text="QUITTER",command = self.stop)
        self.bouton_quitter.grid(column=0,row=1,sticky='EW',pady=10,padx=3)
        
        
        # Disabling resizing
        #self.parent.resizable(0,0)
        
        # resizing: 
        self.parent.grid_columnconfigure(0,weight=1)
        self.parent.grid_rowconfigure(0,weight=1)
        self.grid_rowconfigure(0,weight=1)
        self.tabs.grid_rowconfigure(0,weight=1)
        self.tabs.grid_columnconfigure(0,weight=1)
        self.measurement_tab.grid_rowconfigure(0,weight=1, minsize=120)
        self.measurement_tab.grid_rowconfigure(1,weight=2)
        
        self.parent.minsize(width=350, height=500)
        
        # Start model
        self.model.start()
        
        #binds: 
        self.parent.bind('<Return>', self.frame_ctrl.stop_car)
        
    def stop(self):
        self.model.stop()
        
        if self.model.isAlive():
            self.model.join(0.1)
            
        if self.model.isAlive():
            self.model.join(1)

        if self.model.isAlive():
            print("--- Model thread not properly joined.")
            
        self.parent.destroy()        
            
"""
Test functions
"""
def test_new_log_value():
    x = 0;
    print("logger test started.")
    for i in range(0, 256):
        test = list()
        value = sin(x)
        test.append(value)
        x += 0.05
        pub.sendMessage('var_value_update',varid=0,value=test)

def printout_char(rxbyte):
    print(rxbyte)

"""
Program startup
"""
if __name__ == '__main__':
    # Create window
    root = Tk.Tk()    
    root.geometry('+0+0')

    pub.subscribe(printout_char,'new_ignored_rx_byte')
    
    app = Application(root,width=640, height=480)
    app.grid()
    
    root.protocol('WM_DELETE_WINDOW', app.stop)
    app.mainloop()
    print("Done.")
