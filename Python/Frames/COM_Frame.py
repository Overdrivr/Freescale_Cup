# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import tkinter as Tk
import tkinter.ttk as ttk
from pubsub import pub
import numpy as np
from collections import deque

"""
COM GUI Frame
"""

class COM_Frame(ttk.LabelFrame):
    def __init__(self,parent,model,**kwargs):
        ttk.LabelFrame.__init__(self,parent,text="COM Ports",**kwargs)
        self.parent = parent
        self.model = model
        self.connected = False
        
        self.grid(row=0,column=0,sticky="WENS")
         
        #Widgets
        self.txt_ports = ttk.Label(self,text="STATUS :",style="BW.TLabel")
        self.txt_ports.grid(column=0,row=0,sticky='EW',pady=3,padx=3)

        #
        self.listbox_frame = ttk.Frame(self)
        self.listbox_frame.grid(column=0,row=1,sticky='NSEW',pady=3,padx=3,columnspan=3)

        self.liste = Tk.Listbox(self.listbox_frame,height=3,width=40)
        self.liste.grid(column=0,row=0,sticky="WENS")

        self.scrollbar_liste = ttk.Scrollbar(self.listbox_frame)
        self.scrollbar_liste.config(command = self.liste.yview)
        self.liste.config(yscrollcommand = self.scrollbar_liste.set)
        self.scrollbar_liste.grid(column=1,row=0,sticky="WNS")

        #
        self.bouton_refresh_ports = ttk.Button(self, text="REFRESH", command = self.refresh_COM_ports)
        self.bouton_refresh_ports.grid(column=0,row=2,sticky='EW',pady=3,padx=3)

        self.bouton_connect = ttk.Button(self, text="CONNECT", command = self.start_com)
        self.bouton_connect.grid(column=1,row=2,sticky='EW',pady=3,padx=3)

        self.bouton_disconnect = ttk.Button(self, text="DISCONNECT", command = self.stop_com)
        self.bouton_disconnect.grid(column=2,row=2,sticky='EW',pady=3,padx=3)
        
        self.txt_connected = Tk.Label(self,text="NOT CONNECTED",fg='red')
        self.txt_connected.grid(column=2,row=0,sticky='E')

        #redimensionnement:
        self.parent.grid_columnconfigure(0,weight=1)
        self.parent.grid_rowconfigure(0,weight=1)

        self.grid_columnconfigure(1, weight=1)
        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(2, weight=1)
        self.grid_rowconfigure(1, weight=1, minsize=40)
        
        self.listbox_frame.grid_columnconfigure(0,weight=1)
        self.listbox_frame.grid_rowconfigure(0,weight=1)
        
        
        #Subscriptions
        pub.subscribe(self.com_connected,'com_port_connected')
        pub.subscribe(self.com_disconnected,'com_port_disconnected')

        if __name__!="__main__":
            self.refresh_COM_ports()

    def refresh_COM_ports(self):
        ports_list = self.model.get_ports()
        self.liste.delete(0,Tk.END)
            
        print('COM ports list :') 
        for p, desc, hwid in sorted(ports_list):
            print('--- %s %s\n' % (p, desc))
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

    def stop_com(self):
        self.model.disconnect_com()
        
    def com_connected(self,port):
        self.change_COM_state(state="connected")
        self.connected = True
        #Autoselect first port
        self.liste.selection_set(0)

    def com_disconnected(self):
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
        
if __name__=="__main__":
    root = Tk.Tk() 
    COM_frm = COM_Frame(root,None)
    root.minsize(width=300, height=120)
    root.mainloop()       
