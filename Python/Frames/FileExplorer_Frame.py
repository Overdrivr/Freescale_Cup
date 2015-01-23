# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import tkinter as Tk
import tkinter.ttk as ttk
from pubsub import pub
import numpy as np
from collections import deque

"""
File Explorer Frame
"""

class FileExplorer_Frame(ttk.Frame):
    def __init__(self,parent,model,**kwargs):
        ttk.Frame.__init__(self,parent,**kwargs)
        self.parent = parent
        self.model = model
        self.connected = False
        
        self.grid(row=0,column=0,sticky="WENS")
         
        #Widgets

        #
        self.bouton_open = ttk.Button(self, text="OPEN", command = self.open_measurement)
        self.bouton_open.grid(column=0,row=0,sticky='NSEW',columnspan=3,pady=3,padx=3)
        
        self.txt_ports = ttk.Label(self,text="File :")
        self.txt_ports.grid(column=0,row=1,sticky='NEW',pady=3,padx=3)

        self.selected_filename = Tk.StringVar()
        self.selected_filename.set("None")

        self.txt_ports = ttk.Label(self,textvariable=self.selected_filename)
        self.txt_ports.grid(column=1,row=1,sticky='NEW',pady=3,padx=3)

        #
        self.listbox_frame = ttk.LabelFrame(self,text="Variables")
        self.listbox_frame.grid(column=0,row=2,sticky='NEW',pady=3,padx=3,columnspan=3)

        self.liste = Tk.Listbox(self.listbox_frame,height=25,width=40)
        self.liste.grid(column=0,row=0,sticky="WEN")

        self.scrollbar_liste = ttk.Scrollbar(self.listbox_frame)
        self.scrollbar_liste.config(command = self.liste.yview)
        self.liste.config(yscrollcommand = self.scrollbar_liste.set)
        self.scrollbar_liste.grid(column=1,row=0,sticky="NS")

        #
        self.bouton_plot = ttk.Button(self, text="PLOT", command = self.plot_selection)
        self.bouton_plot.grid(column=0,row=3,sticky='NEW',pady=3,padx=3,columnspan=3)

        self.bouton_previous = ttk.Button(self, text="<", command = self.previous_point)
        self.bouton_previous.grid(column=0,row=4,sticky='NEW',rowspan=2,pady=3,padx=3)

        self.bouton_next = ttk.Button(self, text=">", command = self.next_point)
        self.bouton_next.grid(column=2,row=4,sticky='NEW',rowspan=2,pady=3,padx=3)
        
        self.time_info = ttk.Label(self,text="Time (Arrays only)")
        self.time_info.grid(column=1,row=4,sticky='E')

        self.current_time = Tk.DoubleVar()
        self.current_time.set(0.0)
        
        self.current_time_display = ttk.Label(self,textvariable=self.current_time)
        self.current_time_display.grid(column=1,row=5,sticky='E')

        #redimensionnement:
        self.parent.grid_columnconfigure(0,weight=1)
        self.parent.grid_rowconfigure(0,weight=1)

        self.grid_columnconfigure(1, weight=1)
        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(2, weight=1)
        self.grid_rowconfigure(1, weight=1)
        
        self.listbox_frame.grid_columnconfigure(0,weight=1)
        self.listbox_frame.grid_rowconfigure(0,weight=1)        

    def open_measurement(self):
        pass

    def plot_selection(self):
        pass

    def previous_point(self):
        pass

    def next_point(self):
        pass
        
if __name__=="__main__":
    root = Tk.Tk() 
    frm = FileExplorer_Frame(root,None)
    root.minsize(width=300, height=100)
    root.mainloop()        
