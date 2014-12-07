from pubsub import pub
from SerialPortHandler import SerialPortHandler
from Logger import Logger
from SerialProtocol import SerialProtocol

# Top-level API
#TODO : Log var API
#TODO : Rename start_logger in request_table ?
#TODO : Remove stop_logger ?

class Model():
    def __init__(self, **kwargs):        
       
        # Serial thread
        self.serialthread = SerialPortHandler()
        # Logger
        self.logger = Logger()
        # Serial protocol
        self.serial_protocol = SerialProtocol()
        
        
    def get_ports(self):
        return self.serialthread.get_ports()
        
    def connect_com(self,COM_port):
        # Connect
        self.serialthread.connect(COM_port,115200)
        # Start serial thread 
        self.serialthread.start()

    def disconnect_com(self):
        self.serialthread.close()
         
    def stop(self):
        self.stop_logger()
        self.serialthread.stop()

        if self.serialthread.isAlive():
            self.serialthread.join(0.1)
            
        if self.serialthread.isAlive():
            self.serialthread.join(1)

        if self.serialthread.isAlive():
            print("--- Thread not properly joined.")
        else:
            print("--- Thread stopped.")
        
    def start_logger(self):
        #Get command for querying variable table MCU side
        cmd = self.logger.get_table_cmd()
        #Feed command to serial protocol payload processor
        frame = self.serial_protocol.process_tx_payload(cmd)        
        #Send command
        if self.serialthread.isAlive():
            self.serialthread.write(frame)      
        
    def stop_logger(self):
        # Tell MCU to stop sending data ?
        
        print('--- Logger stopped.')

    # Tell MCU to return variable 'varid' at given interval
    def log_var(self, varid):        
        # Get command
        cmd = self.logger.get_read_cmd(varid)
        
        # Feed command to serial protocol payload processor
        frame = self.serial_protocol.process_tx_payload(cmd)
        
        # Send command
        if self.serialthread.isAlive():
            self.serialthread.write(frame)

    def write_to_var(self,varid,value):
        # Get command
        cmd = self.logger.get_write_cmd(varid,value)
        
        if cmd == None:
            return
        # Feed command to serial protocol payload processor
        frame = self.serial_protocol.process_tx_payload(cmd)
        
        # Send command
        if self.serialthread.isAlive():
            self.serialthread.write(frame)
        
    
# List of events that can be subscribed to
"""
--- Serial port (SerialPortHandler.py)
    * When COM port is connected : 'com_port_connected',port
    * When COM port is disconnected : 'com_port_disconnected'
    * When a new byte is received from COM port : 'new_rx_byte',rxbyte
    
--- Serial protocol (SerialProtocol.py)
    * When a new payload has been decoded by the serial protocol : 'new_rx_payload',rxpayload
    * When a character is not part of a message on the serial port : 'new_ignored_rx_byte',rxbyte

--- Logger (Logger.py)
    * When variable table has been received : 'logtable_update',varlist
    * When variable value has been received : 'var_value_update',varid,value_list
"""
