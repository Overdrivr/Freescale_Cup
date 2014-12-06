import tkinter as Tk
import random
import sys
from threading import Thread
import time
import ttk as ttk
from pubsub import pub
import numpy as np
from collections import deque




"""
Logger GUI Frame
"""
class Logger_Frame(Tk.Frame):
    def __init__(self, parent,model,**kwargs):
        Tk.Frame.__init__(self,parent,**kwargs)
        self.parent = parent
        self.model = model

        pub.subscribe(self.listener_table_received,'logtable_update')

        self.txt_log = Tk.Label(self,text="LOGGER")
        self.txt_log.grid(column=0,row=0,sticky='EW',pady=3,padx=3)

        self.txt_active = Tk.Label(self,text="INACTIVE",fg='blue',borderwidth=2)
        self.txt_active.grid(column=1,row=0,sticky='EW',pady=3,padx=3)

        self.bouton_activate = Tk.Button(self, text="RETRIEVE TABLE", command = self.activate_log)
        self.bouton_activate.grid(column=0,row=2,sticky='EW',pady=3,padx=3)

        self.var_list = ttk.Treeview(self, show="headings",columns=("name","type","size"))
        self.var_list.grid(column=0,row=3,sticky='EW',columnspan=2,pady=3,padx=3)
        self.var_list.column('name',anchor='center',minwidth=0,width=100)
        self.var_list.heading('name', text='name')
        self.var_list.column('type',anchor='center',minwidth=0,width=100, stretch=Tk.NO)
        self.var_list.heading('type', text='type')
        self.var_list.column('size',anchor='center',minwidth=0,width=100, stretch=Tk.NO)
        self.var_list.heading('size', text='size')

        self.value = Tk.DoubleVar()
        self.value.set(0.0)

        self.entry = Tk.Entry(self,width=6,textvariable=self.value)
        self.entry.grid(column = 0, row = 4, sticky="EW",padx=3,pady=3)
        
        self.bouton_write = Tk.Button(self, text="WRITE", command = self.write_value)
        self.bouton_write.grid(column=1,row=4,sticky='EW',pady=3,padx=3)

    def activate_log(self):
        # Activate serial data interception
        self.change_state("inprocess")
        # Start logger
        self.model.start_logger()
        
    def listener_table_received(self,varlist):
        # Signal new state
        self.change_state(state="active")
        # Empty table
        x = self.var_list.get_children()
        for item in x:
            self.var_list.delete(item)
        # Fill table with new values
        for item in varlist:
            i = self.var_list.insert('','end')
            self.var_list.set(i,'name',item[4])
            self.var_list.set(i,'type',item[1])
            self.var_list.set(i,'size',item[2])
            
    def change_state(self,state):
        if state == "inprocess":
            self.txt_active.config(text="WAITING TABLE",fg="orange")
        elif state == "active":
            self.txt_active.config(text="ACTIVE",fg='green')
        else:
            self.txt_active.config(text="INACTIVE",fg='blue')
        self.parent.update_idletasks()

    def write_value(self):
        # Find selected variable

        # Check rights

        # Get entry value

        # Tell API to write value
        self.model.write_to_var(0,0)
