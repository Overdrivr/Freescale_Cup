# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import matplotlib, sys
matplotlib.use('TkAgg')
from numpy import arange, sin, pi
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
 
import tkinter as Tk
import tkinter.ttk as ttk
from pubsub import pub
import numpy as np
from collections import deque

"""
2D Plot GUI Frame
"""
class Plot2D_Frame(Tk.Frame):
    def __init__(self,parent,model,tkmaster,logger_frame,**kwargs):
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
        self.logger_frame = logger_frame

        pub.subscribe(self.listener_new_value_received,'var_value_update')
        #pub.subscribe(self.listener_var_selected,'new_var_selected')

        # Widgets
        self.grid(row=0,column=0,sticky="WENS")
        
        self.liste = Tk.Listbox(self,height=1)
        self.liste.grid(column=1,row=2,sticky='EW',pady=3,padx=3,rowspan=2)
        
        self.scrollbar_liste = Tk.Scrollbar(self.liste)
        self.scrollbar_liste.config(command = self.liste.yview)
        self.liste.config(yscrollcommand = self.scrollbar_liste.set)
        self.scrollbar_liste.pack(side=Tk.RIGHT)

        #
        self.bouton_add_var = Tk.Button(self, text="PLOT SELECTION", command = self.add_var_to_plot)
        self.bouton_add_var.grid(column=0,row=2,rowspan=2,pady=3,padx=3,sticky='NSEW')

        #
        self.bouton_switch_mode = Tk.Button(self, text="REMOVE VAR", command = self.remove_var_from_plot)
        self.bouton_switch_mode.grid(column=2,row=3,pady=3,padx=3)

        #
        self.bouton_Clear = Tk.Button(self, text="Clear", command = self.clear_plot)
        self.bouton_Clear.grid(column=3,row=3,pady=3,padx=3)
        
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
        self.plotFrame = Tk.Frame(self)
        self.dataPlot = FigureCanvasTkAgg(self.f, master=self)
        self.dataPlot.show()
        self.dataPlot.get_tk_widget().grid(column=0,row=0,sticky='WENS',columnspan=5)
      
        #
        self.toolbar = NavigationToolbar2TkAgg(self.dataPlot, self.plotFrame)
        self.plotFrame.grid(row=1,column=0,columnspan=5,sticky="WENS")
     
        #
        self.selected_var_name = Tk.StringVar()
        self.selected_var_name.set("No variable")
        self.selected_var = Tk.Label(self,textvariable=self.selected_var_name,bd=2,relief=Tk.GROOVE)
        self.selected_var.grid(column=2,row=2,sticky='EW',pady=3,padx=3)
        #
        self.selected_value = Tk.DoubleVar()
        self.selected_value.set(0.0)
        self.selected_var_val = Tk.Label(self,textvariable=self.selected_value,bd=2,relief=Tk.GROOVE)
        self.selected_var_val.grid(column=3,row=2,sticky='EW',pady=3,padx=3)

        #redimensionnement:
        self.parent.grid_columnconfigure(0,weight=1)
        self.parent.grid_rowconfigure(0,weight=1)
        
        self.grid_columnconfigure(1, weight=1)
        #self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        
    """    
    def listener_var_selected(self,varid,varname):
        self.selected_varid = varid
        self.selected_varname = varname
        
    """
    def listener_new_value_received(self,varid,data):
        if not varid == self.plotted_varid:
            return
        
        #Update plot with new value if name is found
        if len(data['values']) == 1:

            if self.first:
                self.ymin = data['values'][0]
                self.ymax = data['values'][0]
                self.first = False
            else:
                self.ymin = np.minimum(self.ymin,data['values'][0])
         #      self.ymax = np.maximum(self.ymax,data['values'][0])

            # Check min != max 
            if self.ymin == self.ymax :
                if self.ymin == 0:
                    self.ymin -= 0.0001
                    self.ymax += 0.0001
                else:
                    self.ymax *= 1.0001
                    
            self.a.set_ylim([self.ymin - 0.1 * np.abs(self.ymin), self.ymax + 0.1 * np.abs(self.ymax)])
            
         #   self.x.appendleft(data['time'])
            self.y.appendleft(data['values'][0])
            
          #  xmin = np.amin(self.x)
         #   xmax = np.amax(self.x)

             # Check min != max 
         #   if xmin == xmax :
         #       if xmin == 0:
         #           xmin -= 0.0001
        #            xmax += 0.0001
        #        else:
        #            xmax *= 1.0001
                    
         #   self.a.set_xlim(xmin,xmax)
            
            self.line1.set_data(np.arange(len(self.y))[::-1],self.y)
            self.dataPlot.draw()

            self.selected_value.set(data['values'][0])
        else:
            # TODO : Bound max refresh rate
            if self.first:
                self.ymin = data['values'][0]
                self.ymax = data['values'][0]
                self.first = False

            data['values'].append(self.ymin)
            data['values'].append(self.ymax)
            
            self.ymin = np.amin(data['values'])
            self.ymax = np.amax(data['values'])

            # Check min != max 
            if self.ymin == self.ymax :
                if self.ymin == 0:
                    self.ymin -= 0.0001
                    self.ymax += 0.0001
                else:
                    self.ymax *= 1.0001
                    
            data['values'].pop()
            data['values'].pop()
            
            self.a.set_ylim([self.ymin - 0.1 * np.abs(self.ymin), self.ymax + 0.1 * np.abs(self.ymax)])
            self.a.set_xlim(0,len(data['values']))
            self.line1.set_data(np.arange(len(data['values']))[::-1],data['values'])
            self.dataPlot.draw()

            self.selected_value.set(data['values'][0])
    
    def add_var_to_plot(self):
        variable_id = self.logger_frame.selected_var_id
        
        if not variable_id:
            return
        # Tell Variable manager we are stopped with former var
        # and we need the new one
        pub.sendMessage('stop_using_var',varid=self.plotted_varid)        
        pub.sendMessage('using_var',varid=variable_id)
        
        self.plotted_varid = variable_id
        
        self.selected_var_name.set(self.selected_varname)
        self.first = True

    def remove_var_from_plot(self):
        #Remove selected var
        #Add var to available variables
        pass
        
           
    def clear_plot(self):
        print("hello")#Clearplot
        pass

    def stop(self):
        pub.sendMessage('stop_using_var',varid=self.plotted_varid)
        self.parent.destroy()
        
if __name__=="__main__":
    root = Tk.Tk() 
    Plot_frm = Plot2D_Frame(root,None, root)
    root.minsize(width=300, height=200)
    root.mainloop()
