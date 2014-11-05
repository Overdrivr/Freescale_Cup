#import matplotlib
#matplotlib.use('TkAgg')

#from numpy import arange, sin, pi
#from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg

import tkinter as Tk
import random
import sys
from threading import Thread
import time
import ttk as ttk

"""
COM GUI Frame
"""
class COM_Frame(Tk.Frame):
    def __init__(self,parent,model,**kwargs):
        Tk.Frame.__init__(self,parent,**kwargs)
        self.parent = parent
        self.model = model
        self.connected = False
        
        #Widgets
        self.txt_ports = Tk.Label(self,text="COM PORTS",borderwidth=2,relief=Tk.GROOVE)
        self.txt_ports.grid(column=0,row=0,sticky='EW',pady=3,padx=3)

        self.liste = Tk.Listbox(self,height=1)
        self.liste.grid(column=0,row=1,sticky='EW',columnspan=2)

        self.scrollbar_liste = Tk.Scrollbar(self.liste)
        self.scrollbar_liste.config(command = self.liste.yview)
        self.liste.config(yscrollcommand = self.scrollbar_liste.set)
        self.scrollbar_liste.pack(side=Tk.RIGHT)
        
        self.bouton_refresh_ports = Tk.Button(self, text="REFRESH", command = self.set_COM_ports)
        self.bouton_refresh_ports.grid(column=0,row=2,sticky='EW',pady=3,padx=3)

        self.bouton_connect = Tk.Button(self, text="CONNECT", command = self.start_com)
        self.bouton_connect.grid(column=1,row=2,sticky='EW',pady=3,padx=3)
        
        self.txt_connected = Tk.Label(self,text="NOT CONNECTED",fg='red',width=20)
        self.txt_connected.grid(column=1,row=0,sticky='EW')

    def set_COM_ports(self):
        ports_list = self.model.get_ports()
        self.liste.delete(0,Tk.END)
        for p, desc, hwid in sorted(ports_list):
            self.liste.insert(Tk.END,p)
        pass

    def start_com(self):
        if self.connected:
            return
        
        self.change_COM_state(state="connecting")
        
        if not self.liste.curselection():
            print("No port selected, aborting.")
            time.sleep(0.5)
            self.change_COM_state(state="noconnect")
            return
                  
        chosen_port = self.liste.get(Tk.ACTIVE)

        self.model.start_com(chosen_port)   

        self.change_COM_state(state="connected")

        self.connected = True

    def stop_com():
        pass
    
    def change_COM_state(self,state):
        if state == "connecting":
            self.txt_connected.config(text="CONNECTING",fg="orange")
        elif state == "noconnect":
            self.txt_connected.config(text="NO CONNECT",fg='red')
        else:
            self.txt_connected.config(text="CONNECTED",fg='green')
        self.parent.update_idletasks()
            

"""
Logger GUI Frame
"""
class Logger_Frame(Tk.Frame):
    def __init__(self, parent,model,**kwargs):
        Tk.Frame.__init__(self,parent,**kwargs)
        self.parent = parent
        self.model = model

        self.txt_log = Tk.Label(self,text="LOGGER",borderwidth=2,relief=Tk.GROOVE)
        self.txt_log.grid(column=0,row=0,sticky='EW',pady=3,padx=3)

        self.txt_active = Tk.Label(self,text="INACTIVE",fg='blue',width = 20,borderwidth=2)
        self.txt_active.grid(column=1,row=0,sticky='EW')

        self.bouton_activate = Tk.Button(self, text="RETRIEVE TABLE", command = self.activate_log)
        self.bouton_activate.grid(column=0,row=2,sticky='EW',pady=3,padx=3)

        self.var_list = ttk.Treeview(self, show="headings",columns=("name","type"))
        self.var_list.grid(column=0,row=3,sticky='EW',columnspan=2)
        self.var_list.column('name', width=15, anchor='center')
        self.var_list.heading('name', text='name')
        self.var_list.column('type', width=5, anchor='center')
        self.var_list.heading('type', text='type')

        #Test treeview
        self.var_list.insert('', 'end','row1')
        self.var_list.set('row1','name','filtered_output')
        self.var_list.set('row1','type','Float')

    def activate_log(self):
        #Activate serial data interception
        self.change_state("inprocess")
        pass
        
    def listener_table_received(self,table):
        # Signal new state
        self.change_state(state="active")
        # Empty table
        x = self.var_list.get_children()
        for item in x:
            self.var_list.delete(item)
        # Fill table with new values
        for item in table:
            i = self.var_list.insert('','end')
            self.var_list.set(i,'name',item[0])
            self.var_list.set(i,'type',item[1])
            
    def change_state(self,state):
        if state == "inprocess":
            self.txt_active.config(text="WAITING TABLE",fg="orange")
        elif state == "active":
            self.txt_active.config(text="ACTIVE",fg='green')
        else:
            self.txt_active.config(text="INACTIVE",fg='blue')
        self.parent.update_idletasks()

        
            
