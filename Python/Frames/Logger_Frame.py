# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import tkinter as Tk
import ttk as ttk
from pubsub import pub

"""
Logger GUI Frame
"""

#TODO : Graph should ask logger for selected var

class Logger_Frame(Tk.Frame):
    def __init__(self,parent,model,**kwargs):
        Tk.Frame.__init__(self,parent,**kwargs)
        self.parent = parent
        self.model = model
        self.displayed_var_id = None

        self.txt_log = Tk.Label(self,text="LOGGER")
        self.txt_log.grid(column=0,row=0,sticky='EW',pady=3,padx=3)

        self.txt_active = Tk.Label(self,text="INACTIVE",fg='blue',borderwidth=2)
        self.txt_active.grid(column=1,row=0,sticky='EW',pady=3,padx=3)

        self.bouton_activate = Tk.Button(self, text="RETRIEVE TABLE", command = self.activate_log)
        self.bouton_activate.grid(column=0,row=2,sticky='EW',pady=3,padx=3)

        self.var_list = ttk.Treeview(self, show="headings",columns=("name","type","size","ID"),selectmode="browse")
        self.var_list.grid(column=0,row=3,sticky='EW',columnspan=2,pady=3,padx=3)
        self.var_list.column('name',anchor='center',minwidth=0,width=100)
        self.var_list.heading('name', text='name')
        self.var_list.column('type',anchor='center',minwidth=0,width=100, stretch=Tk.NO)
        self.var_list.heading('type', text='type')
        self.var_list.column('size',anchor='center',minwidth=0,width=100, stretch=Tk.NO)
        self.var_list.heading('size', text='size')
        self.var_list.column('ID',anchor='center',minwidth=0,width=3, stretch=Tk.NO)
        self.var_list.heading('ID', text='ID')
        self.var_list.bind("<<TreeviewSelect>>", self.variable_selected)

        self.variables = dict()

        #Variable name
        self.variable = Tk.StringVar()
        self.variable.set("Variable : ")
        
        self.selected_var = ttk.Label(self,textvariable=self.variable)
        self.selected_var.grid(column=0,row=4)

        #Variable read value
        self.read_val = Tk.DoubleVar()
        
        self.selected_value = ttk.Label(self,textvariable=self.read_val)
        self.selected_value.grid(column=1,row=4)

        #Variable write value
        self.value = Tk.DoubleVar()
        self.value.set(0.0)

        self.entry = Tk.Entry(self,width=6,textvariable=self.value)
        self.entry.grid(column=0, row=5, sticky="EW",padx=3,pady=3)
        
        self.bouton_write = Tk.Button(self, text="WRITE", command = self.write_value)
        self.bouton_write.grid(column=1,row=5,sticky='EW',pady=3,padx=3)

        # Subscriptions
        pub.subscribe(self.listener_COM_connected,'com_port_connected')
        pub.subscribe(self.listener_value_received,'var_value_update')
        pub.subscribe(self.listener_table_received,'logtable_update')

    def activate_log(self):
        # Activate serial data interception
        self.change_state("inprocess")
        # Start logger
        self.model.start_controller()

    def listener_COM_connected(self,port):
        # We activate the distant IO controller immediately after the COM port was connected
        self.activate_log()
        
    def listener_table_received(self,varlist):
        #pub.sendMessage("new_var_selected",varid=None)#TO CHECK IF WORKS
        self.variables = varlist
        
        # Signal new state
        self.change_state(state="active")
        
        # Empty table
        x = self.var_list.get_children()
        for item in x:
            self.var_list.delete(item)
        
        # Fill table with new values
        for key in self.variables:
            i = self.var_list.insert('','end')
            self.var_list.set(i,'name',self.variables[key]['name'])
            self.var_list.set(i,'type',self.variables[key]['datatype'])
            self.var_list.set(i,'size',self.variables[key]['octets'])
            self.var_list.set(i,'ID',key)

    def listener_value_received(self,varid,data):
        if self.displayed_var_id is None:
            return

        if not self.displayed_var_id == varid:
            return

        self.read_val.set(round(data['values'][0],6))        
            
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
        it = self.var_list.selection()
        
        # Get associated var_id   
        var_id = self.var_list.set(it,column='ID')
        
        if not var_id in self.variables:
            print("Logger_Frame error : ID not found :",self.var_list.set(it,column='ID'))
            return
        
        # Get entry value
        value = self.value.get()

        # Tell API to write value
        self.model.write_var(var_id,value)

    def variable_selected(self,event):        
        # Find selected variable
        it = self.var_list.selection()
        
        # Get associated var_id   
        var_id = self.var_list.set(it,column='ID')
        
        if not var_id in self.variables:
            print("Logger_Frame error : ID not found :",self.var_list.set(it,column='ID'))
            return

        # Tell Variable manager we are stopped with former var
        # and we need the new one
        pub.sendMessage('stop_using_var',varid=self.displayed_var_id)
        pub.sendMessage('using_var',varid=var_id)

        self.displayed_var_id = var_id

        # If selected variable is writeable
        if self.variables[var_id]['writeable']:
            self.variable.set(self.variables[var_id]['name'])                      
        else:
            self.variable.set("** Variable not writeable **")
                      
        pub.sendMessage("new_var_selected",varid=var_id,varname=self.variables[var_id]['name'])

        
        
