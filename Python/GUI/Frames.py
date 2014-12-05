import matplotlib, sys
matplotlib.use('TkAgg')
from numpy import arange, sin, pi
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
from mpl_toolkits.mplot3d import Axes3D

import tkinter as Tk
import random
import sys
from threading import Thread
import time
import ttk as ttk
from pubsub import pub
from time import time
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

"""
Graph GUI Frame
"""
class Graph_Frame(Tk.Frame):
    def __init__(self,parent,model,tkmaster,**kwargs):
        Tk.Frame.__init__(self,parent,**kwargs)
        self.parent = parent
        self.model = model
        self.log_vars = list()
        self.plotmode = 'scalar'
        self.index=0
        self.tkmaster = tkmaster
        self.plotted_varid = None

        pub.subscribe(self.listener_table_received,'logtable_update')
        pub.subscribe(self.listener_new_value_received,'var_value_update')

        # Widgets
        self.liste = Tk.Listbox(self,height=1)
        self.liste.grid(column=0,row=1,sticky='EW',pady=3,padx=3,rowspan=2)
        
        self.scrollbar_liste = Tk.Scrollbar(self.liste)
        self.scrollbar_liste.config(command = self.liste.yview)
        self.liste.config(yscrollcommand = self.scrollbar_liste.set)
        self.scrollbar_liste.pack(side=Tk.RIGHT)

        #
        self.bouton_add_var = Tk.Button(self, text="PLOT", command = self.add_var_to_plot)
        self.bouton_add_var.grid(column=1,row=2,pady=3,padx=3)

        #
        self.bouton_switch_mode = Tk.Button(self, text="SWITCH MODE", command = self.switch_plot_mode)
        self.bouton_switch_mode.grid(column=2,row=2,pady=3,padx=3)

        #
        self.f = Figure(figsize=(5,4), dpi=100)
        self.a = self.f.add_subplot(111)
        self.a.set_xlim([0, 127])
        self.a.set_ylim([-255, 255])
        self.line1, = self.a.plot([],[])
        self.x = deque(maxlen=128)
        self.y = deque(maxlen=128)
        self.ymin = 0
        self.ymax = 1
        self.first = False

        #
        self.dataPlot = FigureCanvasTkAgg(self.f, master=self)
        self.dataPlot.show()
        self.dataPlot.get_tk_widget().grid(column=0,row=0,sticky='EW',pady=3,padx=3,columnspan=5)

        #
        self.selected_var = Tk.Label(self,text="no variable",bd=2,relief=Tk.GROOVE)
        self.selected_var.grid(column=1,row=1,sticky='EW',pady=3,padx=3)

        #
        self.ymin_entry = Tk.Entry(self)
        self.ymin_entry.grid(column=3,row=1,sticky='EW',pady=3,padx=3)

        #
        self.ymax_entry = Tk.Entry(self)
        self.ymax_entry.grid(column=3,row=2,sticky='EW',pady=3,padx=3)



    def listener_table_received(self,varlist):
        self.log_vars = varlist
        self.liste.delete(0,Tk.END)
        for item in varlist:
            self.liste.insert(Tk.END,item[4])

    def listener_new_value_received(self,varid,value_list):
        if not varid == self.plotted_varid:
            return
        
        #TODO : Compute min max
        #Update plot with new value if name is found
        if self.plotmode == "scalar":

            if self.first:
                self.ymin = value_list[0]
                self.ymax = value_list[0]
                self.first = False
            else:
                self.ymin = np.minimum(self.ymin,value_list[0])
                self.ymax = np.maximum(self.ymax,value_list[0])
            

            self.a.set_ylim([self.ymin - 0.1 * np.abs(self.ymin), self.ymax + 0.1 * np.abs(self.ymax)])
            
            self.y.appendleft(value_list[0])
            self.line1.set_data(np.arange(len(self.y))[::-1],self.y)
            self.dataPlot.draw()

    def switch_plot_mode(self):
        #TODO : Switch x axis between time and array index
        pass
    
    def add_var_to_plot(self):
        if not self.liste.curselection():
            return
        self.plotted_varid = int(self.liste.curselection()[0])
        self.model.log_var(self.plotted_varid)
        self.first = True

    def remove_var_from_plot(self):
        #Remove selected var
        #Add var to available variables
        pass

"""
3D Graph GUI Frame
"""
class Graph3D_Frame(Tk.Frame):
    def __init__(self,parent,model,tkmaster,**kwargs):
        Tk.Frame.__init__(self,parent,**kwargs)
        self.parent = parent
        self.model = model
        self.log_vars = list()
        self.plotmode = 'scalar'
        self.index=0
        self.tkmaster = tkmaster
        self.plotted_varid = None

        pub.subscribe(self.listener_table_received,'logtable_update')
        pub.subscribe(self.listener_new_value_received,'var_value_update')

        # Widgets
        self.liste = Tk.Listbox(self,height=1)
        self.liste.grid(column=0,row=1,sticky='EW',pady=3,padx=3,rowspan=2)
        
        self.scrollbar_liste = Tk.Scrollbar(self.liste)
        self.scrollbar_liste.config(command = self.liste.yview)
        self.liste.config(yscrollcommand = self.scrollbar_liste.set)
        self.scrollbar_liste.pack(side=Tk.RIGHT)

        #
        self.bouton_add_var = Tk.Button(self, text="PLOT", command = self.add_var_to_plot)
        self.bouton_add_var.grid(column=1,row=2,pady=3,padx=3)

        #
        self.bouton_switch_mode = Tk.Button(self, text="SWITCH MODE", command = self.switch_plot_mode)
        self.bouton_switch_mode.grid(column=2,row=2,pady=3,padx=3)

        #
        self.f = Figure(figsize=(5,4), dpi=100)
        self.a = self.f.add_subplot(111, projection="3d")

        self.data = np.zeros((3,16*128))
        self.a.set_xlim([0, 127])
        self.a.set_ylim([0, 16])
        self.a.set_zlim([0, 100])

        #X, Y = np.meshgrid([1,2,3], [4,5,6,7])

        for y in range(0,16):
            for x in range(0,128):
                self.data[0,y*128+x] = x
                self.data[1,y*128+x] = y
                self.data[2,y*128+x] = y * 0.01
                        
        self.line1, = self.a.plot(self.data[0, 0:1], self.data[1, 0:1], self.data[2, 0:1])

        #
        self.dataPlot = FigureCanvasTkAgg(self.f, master=self)
        self.dataPlot.show()
        self.dataPlot.get_tk_widget().grid(column=0,row=0,sticky='EW',pady=3,padx=3,columnspan=5)
        #Axes3D.mouse_init(self.dataPlot)

        #
        self.selected_var = Tk.Label(self,text="no variable",bd=2,relief=Tk.GROOVE)
        self.selected_var.grid(column=1,row=1,sticky='EW',pady=3,padx=3)

        #
        self.ymin_entry = Tk.Entry(self)
        self.ymin_entry.grid(column=3,row=1,sticky='EW',pady=3,padx=3)

        #
        self.ymax_entry = Tk.Entry(self)
        self.ymax_entry.grid(column=3,row=2,sticky='EW',pady=3,padx=3)



    def listener_table_received(self,varlist):
        self.log_vars = varlist
        self.liste.delete(0,Tk.END)
        for item in varlist:
            self.liste.insert(Tk.END,item[4])

    def listener_new_value_received(self,varid,value_list):
        if not varid == self.plotted_varid:
            return
        
        #TODO : Compute min max
        #Update plot with new value if name is found

            """if self.first:
                self.ymin = value_list[0]
                self.ymax = value_list[0]
                self.first = False
            else:
                self.ymin = np.minimum(self.ymin,value_list[0])
                self.ymax = np.maximum(self.ymax,value_list[0])
            """

        #self.a.set_ylim([self.ymin - 0.1 * np.abs(self.ymin), self.ymax + 0.1 * np.abs(self.ymax)])
        #print(value_list)
        
        self.data[2,:] = np.roll(self.data[2,:],128,axis=0)
        self.data[2,0:128] = value_list
        #print("*****")
        #print(self.data[2,0:128])
        #print("-----")
        #print(self.data[2,128:256])
        #self.line1.set_data(self.x,self.y,self.z)
        #self.a.plot_wireframe(self.x, self.y, self.z, rstride=1, cstride=1)
        self.line1.set_data(self.data[0:2,:256])
        self.line1.set_3d_properties(self.data[2,:256])
        #self.line1.set_segments(self.data)
        self.dataPlot.draw()

    def switch_plot_mode(self):
        #TODO : Switch x axis between time and array index
        pass
    
    def add_var_to_plot(self):
        if not self.liste.curselection():
            return
        self.plotted_varid = int(self.liste.curselection()[0])
        self.model.log_var(self.plotted_varid)
        self.first = True

    def remove_var_from_plot(self):
        #Remove selected var
        #Add var to available variables
        pass
