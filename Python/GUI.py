# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import tkinter as Tk
from Model import Model
from threading import Timer
from pubsub import pub
from array import array
from Frames.COM_Frame import *
from Frames.Logger_Frame import *
from Frames.Plot2D_Frame import *

class Application(Tk.Frame):
        
    def __init__(self, **kwargs):
        #Create window
        self.root = Tk.Tk()
        self.root.geometry('+0+0')
        
        #Init master frame
        Tk.Frame.__init__(self,self.root,width=640, height=480)
        self.pack()

        #Create Model
        self.model = Model()

        #COM Frame
        self.frame_com_ports = COM_Frame(self,self.model,bd=2,relief=Tk.GROOVE)
        self.frame_com_ports.grid(column=0,row=0,sticky='NSEW',pady=5,padx=5)

        #Logger frame
        self.frame_logger = Logger_Frame(self,self.model,bd=2,relief=Tk.GROOVE)
        self.frame_logger.grid(column=0,row=1,sticky='NSEW',pady=5,padx=5)

        #Graph 1 frame
        self.frame_graph1 = Plot2D_Frame(self,self.model,self.root,bd=2,relief=Tk.GROOVE)
        self.frame_graph1.grid(column=1,row=0,sticky='EW',pady=5,padx=5,rowspan=2)

        #Graph 2 frame
        self.frame_graph2 = Plot2D_Frame(self,self.model,self.root,bd=2,relief=Tk.GROOVE)
        self.frame_graph2.grid(column=2,row=0,sticky='EW',pady=5,padx=5,rowspan=2)

        #Quit button
        self.bouton_quitter = Tk.Button(self, text="x",command = self.stop)
        self.bouton_quitter.grid(column=3,row=0,sticky='N')

    def stop(self):
        self.model.stop()

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

def test_rx_table():
    print("logger table test started.")
    c = bytearray()
    #SOF
    c.append(int('7f',16))
    #CMD
    c.append(int('02',16))
    #DATATYPE
    c.append(int('07',16))
    #DATAID
    c.append(int('00',16))
    c.append(int('00',16))
    #Table
        #DATATYPE
    c.append(int('01',16))
        #DATAID
    c.append(int('00',16))
    c.append(int('00',16))
        #DATASIZE
    c.append(int('01',16))
    c.append(int('00',16))
        #NAME
    s = 'test_var                        '
    h = bytearray(s,'ascii')
    c.extend(h)

        #DATATYPE
    c.append(int('03',16))
        #DATAID
    c.append(int('01',16))
    c.append(int('00',16))
        #DATASIZE
    c.append(int('F3',16))
    c.append(int('0F',16))
        #NAME
    s = 'test_array                      '
    h = bytearray(s,'ascii')
    c.extend(h)

    
    #EOF
    c.append(int('7f',16))

    print("Test frame :",c)
    
    for x in c:
        t = x,
        a = bytes(t)
        pub.sendMessage('new_rx_byte',rxbyte=a)

def printout_char(rxbyte):
    print("ignored char : ",rxbyte)

"""
Program startup
"""
if __name__ == '__main__':
    app = Application()
    
    pub.subscribe(printout_char,'new_ignored_rx_byte')
    t = Timer(1.0,test_rx_table)
    #t.start()
    
    app.mainloop()
