# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

import tkinter as Tk
import tkinter.ttk as ttk
from pubsub import pub
from threading import Thread, Lock, Event, Timer

"""
To handle which variables the MCU should send or not
"""

# TODO : Count the attempts to read a variable, and output error after 10 trials 

class VariableManager(Thread):
    def __init__(self,model,**kwargs):
        Thread.__init__(self)
        self.model = model
        self.variables = dict()
        self.running = True
        self.event = Event()
        self.event.clear()
        self.l = Lock()

        # Subscription
        pub.subscribe(self.add_follower,'using_var')
        pub.subscribe(self.remove_follower,'stop_using_var')
        pub.subscribe(self.variable_state_check,'var_value_update')
        pub.subscribe(self.table_received_listener,'logtable_update')
        
    def add_follower(self,varid):
        self.l.acquire()
        if not varid in self.variables:
            self.variables[varid] = dict()
            self.variables[varid]['amount'] = 1
            self.variables[varid]['read_status'] = "off"
        else:
            self.variables[varid]['amount'] += 1        
        self.l.release()
        
    def remove_follower(self,varid):
        self.l.acquire()
        if not varid in self.variables:
            self.l.release()
            return
        
        if self.variables[varid]['amount'] == 0:
            print("VariableManager error : Trying to reduced null follower amount")
            self.l.release()
            return
        
        self.variables[varid]['amount'] -= 1
        self.l.release()

    def table_received_listener(self,varlist):
        self.reset()
        
    def reset(self):
        self.variables = dict()
        
    def variable_state_check(self,varid,data):
        self.l.acquire()
        if varid in self.variables:
            self.variables[varid]['read_status'] = "on"
        else:
            # To fix variables that are received but not needed
            self.variables[varid] = dict()
            self.variables[varid]['read_status'] = "on"
            self.variables[varid]['amount'] = 0
        self.l.release()
        

    def stop(self):
        self.l.acquire()
        self.running = False
        self.variables = dict()
        self.l.release()
        self.event.set()
        
    def run(self):
        while(self.running):
            #Update every 500ms
            self.event.wait(0.5)
            self.l.acquire()
            # Ask MCU to start sending all needed variables variable
            for key, item in self.variables.items():
                if item['read_status'] == "off" and item['amount'] > 0:
                    #print("Start :",key)
                    self.model.read_var(key)
                    
                if item['read_status'] == "on" and item['amount'] == 0:
                    self.model.stop_read_var(key)
                    #print("Stop : ",key)
                    item['read_status'] = "off"
            self.l.release()
            self.event.clear()
