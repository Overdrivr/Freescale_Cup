# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import tkinter as Tk
import ttk as ttk
from pubsub import pub
import numpy as np
from collections import deque

"""
COM GUI Frame
"""
#TODO : Change CONNECT button name and function once connected
#TODO : Plot button should call Model.log_var with selected id
class COM_Frame(Tk.Frame):
    def __init__(self,parent,model,**kwargs):
        Tk.Frame.__init__(self,parent,**kwargs)
        self.parent = parent
        self.model = model
        self.connected = False
        
        #Widgets
        self.txt_ports = Tk.Label(self,text="COM PORTS")
        self.txt_ports.grid(column=0,row=0,sticky='EW',pady=3,padx=3)

        #
        self.listbox_frame = Tk.Frame(self)
        self.listbox_frame.grid(column=0,row=1,sticky='NSEW',pady=3,padx=3,columnspan=3)
        
        self.liste = Tk.Listbox(self.listbox_frame,height=3,width=40)
        self.liste.pack(side = Tk.LEFT,fill=Tk.X)

        self.scrollbar_liste = Tk.Scrollbar(self.listbox_frame)
        self.scrollbar_liste.config(command = self.liste.yview)
        self.liste.config(yscrollcommand = self.scrollbar_liste.set)
        self.scrollbar_liste.pack(side=Tk.RIGHT)

        self.bouton_refresh_ports = Tk.Button(self, text="REFRESH", command = self.set_COM_ports)
        self.bouton_refresh_ports.grid(column=0,row=2,sticky='EW',pady=3,padx=3)

        self.bouton_connect = Tk.Button(self, text="CONNECT", command = self.start_com)
        self.bouton_connect.grid(column=1,row=2,sticky='EW',pady=3,padx=3)
        
        self.txt_connected = Tk.Label(self,text="NOT CONNECTED",fg='red')
        self.txt_connected.grid(column=2,row=0,sticky='EW')

        #Subscriptions
        pub.subscribe(self.com_connected,'com_port_connected')
        pub.subscribe(self.com_disconnected,'com_port_disconnected')

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
            self.change_COM_state(state="noconnect")
            return
                  
        chosen_port = self.liste.get(Tk.ACTIVE)
        self.model.connect_com(chosen_port)

    def stop_com():
        self.model.disconnect_com(self)
        
    def com_connected(self,port):
        #TODO : Change connect button name and function
        self.change_COM_state(state="connected")
        self.connected = True

    def com_disconnected(self):
        #TODO : Change connect button name and function
        self.change_COM_state(state="noconnect")
        self.connected = False
    
    def change_COM_state(self,state):
        if state == "connecting":
            self.txt_connected.config(text="CONNECTING",fg="orange")
        elif state == "noconnect":
            self.txt_connected.config(text="NO CONNECT",fg='red')
        else:
            self.txt_connected.config(text="CONNECTED",fg='green')
        self.parent.update_idletasks()
