import tkinter as Tk
import random
import sys
from threading import Thread
import time
from Frames import *
from Model import Model
from threading import Timer
from pubsub import pub

class Application(Tk.Frame):
        
    def __init__(self, **kwargs):
        #Create window
        self.root = Tk.Tk()

        #Init master frame
        Tk.Frame.__init__(self,self.root,width=640, height=480)
        self.pack()

        #Create Model
        self.model = Model()

        #COM Frame
        self.frame_com_ports = COM_Frame(self,self.model,bd=2,relief=Tk.GROOVE)
        self.frame_com_ports.grid(column=0,row=0,sticky='N',pady=5,padx=5)

        #Logger frame
        self.frame_logger = Logger_Frame(self,self.model,bd=2,relief=Tk.GROOVE)
        self.frame_logger.grid(column=0,row=1,sticky='N',pady=5,padx=5)

        #Graph frame
        self.frame_graph1 = Graph_Frame(self,self.model,self.root,bd=2,relief=Tk.GROOVE)
        self.frame_graph1.grid(column=1,row=0,sticky='EW',pady=5,padx=5,rowspan=2)

        #Quit button
        self.bouton_quitter = Tk.Button(self, text="x",command = self.stop)
        self.bouton_quitter.grid(column=2,row=0,sticky='N')

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

"""
Program startup
"""
if __name__ == '__main__':
    app = Application()
    t = Timer(1.0,test_new_log_value)
    t.start()
    app.mainloop()
