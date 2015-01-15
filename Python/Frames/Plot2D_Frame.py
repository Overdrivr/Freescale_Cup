# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import matplotlib, sys
matplotlib.use('TkAgg')
from numpy import arange, sin, pi
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure

import tkinter as Tk
import ttk as ttk
from pubsub import pub
import numpy as np
from collections import deque

"""
2D Plot GUI Frame
"""
class Plot2D_Frame(Tk.Frame):
    def __init__(self,parent,model,tkmaster,**kwargs):
        Tk.Frame.__init__(self,parent,**kwargs)
        self.parent = parent
        self.model = model
        self.log_vars = list()
        self.plotmode = 'scalar'
        self.index=0
        self.tkmaster = tkmaster
        self.plotted_varid = None
        self.selected_varid = None
        self.selected_varname = ""
        self.s_value = 0.0

        pub.subscribe(self.listener_new_value_received,'var_value_update')
        pub.subscribe(self.listener_var_selected,'new_var_selected')

        # Widgets
        self.liste = Tk.Listbox(self,height=1)
        self.liste.grid(column=1,row=1,sticky='EW',pady=3,padx=3,rowspan=2)
        
        self.scrollbar_liste = Tk.Scrollbar(self.liste)
        self.scrollbar_liste.config(command = self.liste.yview)
        self.liste.config(yscrollcommand = self.scrollbar_liste.set)
        self.scrollbar_liste.pack(side=Tk.RIGHT)

        #
        self.bouton_add_var = Tk.Button(self, text="PLOT SELECTION", command = self.add_var_to_plot)
        self.bouton_add_var.grid(column=0,row=1,rowspan=2,pady=3,padx=3,sticky='NSEW')

        #
        self.bouton_switch_mode = Tk.Button(self, text="REMOVE VAR", command = self.remove_var_from_plot)
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
        self.selected_var_name = Tk.StringVar()
        self.selected_var_name.set("No variable")
        self.selected_var = Tk.Label(self,textvariable=self.selected_var_name,bd=2,relief=Tk.GROOVE)
        self.selected_var.grid(column=2,row=1,sticky='EW',pady=3,padx=3)
        #
        self.selected_value = Tk.DoubleVar()
        self.selected_value.set(0.0)
        self.selected_var_val = Tk.Label(self,textvariable=self.selected_value,bd=2,relief=Tk.GROOVE)
        self.selected_var_val.grid(column=3,row=1,sticky='EW',pady=3,padx=3)

    def listener_var_selected(self,varid,varname):
        self.selected_varid = varid
        self.selected_varname = varname
        

    def listener_new_value_received(self,varid,value_list):
        if not varid == self.plotted_varid:
            return
        
        #Update plot with new value if name is found
        if len(value_list) == 1:

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

            self.selected_value.set(value_list[0])
        else:

            if self.first:
                self.ymin = value_list[0]
                self.ymax = value_list[0]
                self.first = False

            value_list.append(self.ymin)
            value_list.append(self.ymax)
            
            self.ymin = np.amin(value_list)
            self.ymax = np.amax(value_list)

            value_list.pop()
            value_list.pop()
            
            self.a.set_ylim([self.ymin - 0.1 * np.abs(self.ymin), self.ymax + 0.1 * np.abs(self.ymax)])
                
            self.line1.set_data(np.arange(len(value_list))[::-1],value_list)
            self.dataPlot.draw()

            self.selected_value.set(value_list[0])
    
    def add_var_to_plot(self):
        if self.selected_varid == None:
            return
        
        self.plotted_varid = self.selected_varid
        self.selected_var_name.set(self.selected_varname)
        self.model.read_var(self.plotted_varid)
        self.first = True

    def remove_var_from_plot(self):
        #Remove selected var
        #Add var to available variables
        pass
