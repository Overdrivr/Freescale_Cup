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
from Frames.Plot2D_Frame import *

class Application(ttk.Frame):
        
    def __init__(self,parent,**kwargs):
        # Init
        self.parent = parent
        ttk.Frame.__init__(self,parent,**kwargs)
        # Init configuration
        ttk.Style().configure("BW.TLabel")
        ttk.Style().configure("BW.TButton")
        
        # Create Model
        self.model = Model()

        # COM Frame
        self.frame_com_ports = COM_Frame(self,self.model,relief=Tk.GROOVE)
        self.frame_com_ports.grid(column=0,row=0,sticky='NSEW',pady=2,padx=5)

        # Logger frame
        self.frame_logger = Logger_Frame(self,self.model,bd=2,relief=Tk.GROOVE)
        self.frame_logger.grid(column=0,row=1,sticky='NSEW',pady=2,padx=5)

        # Graph 1 frame
        self.frame_graph1 = Plot2D_Frame(self,self.model,self.parent,bd=2,relief=Tk.GROOVE)
        self.frame_graph1.grid(column=1,row=0,sticky='EW',pady=2,padx=0,rowspan=2)

        # Graph 2 frame
        self.frame_graph2 = Plot2D_Frame(self,self.model,self.parent,bd=2,relief=Tk.GROOVE)
        self.frame_graph2.grid(column=2,row=0,sticky='EW',pady=2,padx=0,rowspan=2)

        # Quit button
        self.bouton_quitter = Tk.Button(self, text="QUITTER",command = self.stop)
        self.bouton_quitter.grid(column=0,row=2,sticky='EW',pady=2,padx=5)

        self.model.start()

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
    print("ignored char : ",rxbyte)

"""
Program startup
"""
if __name__ == '__main__':
    # Create window
    root = Tk.Tk()    
    root.geometry('+0+0')
    
    app = Application(root,width=640, height=480)
    app.pack()
    
    root.protocol('WM_DELETE_WINDOW', app.stop)
    app.mainloop()
    print("Done.")
