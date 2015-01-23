# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from pubsub import pub
from SerialPort import SerialPort
from DistantIO import DistantIO
from Protocol import Protocol
from threading import Thread
from VariableManager import VariableManager
from DataLogger import DataLogger

# Top-level API

class Model(Thread):
    def __init__(self, **kwargs):
        Thread.__init__(self)
        # Serial thread
        self.serialthread = SerialPort()
        # Controller Read/Write variables over serial
        self.controller = DistantIO()
        # Serial protocol
        self.protocol = Protocol()
        # Variable manager
        self.variable_manager = VariableManager(self)
        self.variable_manager.start()
        self.running = True;
        # Data logger
        self.logger = DataLogger()
        
    def get_ports(self):
        return self.serialthread.get_ports()
        
    def connect_com(self,COM_port):
        # Start serial thread (can run without COM port connected)
        if not self.serialthread.isAlive():
            self.serialthread.start()
            
        self.serialthread.connect(COM_port,115200)

    #Model update running in a thread
    def run(self):
        while self.running:
            if self.serialthread.char_available():
                c = self.serialthread.get_char()
                if not c is None:
                    self.protocol.process_rx(c)

            if self.protocol.available():
                p =  self.protocol.get()
                # Dump payload if controller is in heavy load ?
                pub.sendMessage('new_rx_payload',rxpayload=p)#USED ?
                if not p is None:
                    self.controller.decode(p)

    def disconnect_com(self):
        self.serialthread.disconnect()
         
    def stop(self):
        self.running = False
        self.serialthread.stop()
        self.variable_manager.stop()

        if self.serialthread.isAlive():
            self.serialthread.join(0.1)
            
        if self.serialthread.isAlive():
            self.serialthread.join(1)

        if self.serialthread.isAlive():
            print("--- Serial thread not properly joined.")

        if self.variable_manager.isAlive():
            self.variable_manager.join(0.1)
            
        if self.variable_manager.isAlive():
            self.variable_manager.join(1)
            
        self.stop_controller()
        
    def start_controller(self):
        #Get command for querying variable table MCU side
        cmd = self.controller.encode(cmd='table')
        #Feed command to serial protocol payload processor
        frame = self.protocol.process_tx(cmd)        
        #Send command
        self.serialthread.write(frame)      
        
    def stop_controller(self):
        #TODO : Tell MCU to stop sending all data
        pass

    def start_log(self):
        self.logger.start()
        
    def stop_log(self):
        self.logger.record_all()

    def read_var(self, varid):        
        # Get command
        cmd = self.controller.encode(cmd='read',var_id=varid)
        # Feed command to serial protocol payload processor
        frame = self.protocol.process_tx(cmd)
        # Send command
        self.serialthread.write(frame)

    def write_var(self,varid,value):
        # Get command
        cmd = self.controller.encode(cmd='write',var_id=varid,value=value)
        if cmd == None:
            return
        # Feed command to serial protocol payload processor
        frame = self.protocol.process_tx(cmd)
        # Send command
        self.serialthread.write(frame)
        
    def stop_read_var(self,varid):
        # Get command
        cmd = self.controller.encode(cmd='stop',var_id=varid)
        if cmd == None:
            return
        # Feed command to serial protocol payload processor
        frame = self.protocol.process_tx(cmd)
        # Send command
        self.serialthread.write(frame)
        
    def stop_read_all_vars():
        # Get command
        cmd = self.controller.encode(cmd='stopall')
        if cmd == None:
            return
        # Feed command to serial protocol payload processor
        frame = self.protocol.process_tx(cmd)
        # Send command
        self.serialthread.write(frame)
        
    def get_var_info(self,varid):
        return self.controller.get_var_info(varid)
        
    
# List of events that can be subscribed to
"""
--- Serial port (SerialPort.py)
    * When COM port is connected : 'com_port_connected',port
    * When COM port is disconnected : 'com_port_disconnected'
    
--- Protocol (Protocol.py)
    * deprecated ?* When a new payload has been decoded by the serial protocol : 'new_rx_payload',rxpayload
    * When a character is not part of a message on the serial port : 'new_ignored_rx_byte',rxbyte

--- DistanIO (DistantIO.py)
    * When variable table has been received : 'logtable_update',varlist
    * When variable value has been received : 'var_value_update',varid,data
    data is a dict
    data['time'] : time value in us where variable was read
    data['values'] : list of values (of size 1 if variable is a scalar)

--- Plot2D_Frame (Plot2D_Frame.py)]
--- Logger_Frame (Plot2D_Frame.py)
    * When a graph or the logger frame has started using a variable : 'using_var',varid
    * When a graph or the logger frame has stopped using a variable : 'stop_using_var',varid

--- GUI
    *DEPRECATED When the selection of a variable to do something with (read, write) changes:
    'new_var_selected',varid,varname
"""
