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
