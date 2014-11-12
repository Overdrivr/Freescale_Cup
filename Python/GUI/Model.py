from pubsub import pub
from SerialPortHandler import SerialPortHandler
from Logger import Logger
from SerialProtocol import SerialProtocol

# Top-level API
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
        
    def start_com(self,COM_port):
        # Connect
        self.serialthread.connect(COM_port,115200)
        # Start serial thread 
        self.serialthread.start()

    def stop_com(self):
        self.stop_logger()
        self.serialthread.stop()

        if self.serialthread.isAlive():
            self.serialthread.join()
            
        print('--- COM stopped.')
         
    def stop(self):
        self.stop_com()
        print("--- All threads stopped.")
        
    def start_logger(self):
        #Get command for querying variable table MCU side
        cmd = self.logger.get_table_cmd()
        #Feed command to serial protocol payload processor
        frame = self.serial_protocol.process_tx_payload(cmd)        
        #Send command
        self.serialthread.write(frame)      
        
    def stop_logger(self):
        # Tell MCU to stop sending data ?
        
        print('--- Logger stopped.')
        
    
# List of events that can be subscribed to
"""
--- Serial port (SerialPortHandler.py)
    * When a new byte is received from COM port : 'new_rx_byte',bytes
    
--- Serial protocol (SerialProtocol.py)
    * When a new payload has been decoded by the serial protocol : 'new_rx_payload',bytesarray
    * When a character is not part of a message on the serial port : 'new_ignored_rx_byte',bytes

--- Logger (Logger.py)
    * When variable table has been received : 'logtable_update',list(varid,datatype,arraysize,write_right,name)
    * When variable value has been received : 'var_value_update',varid,list(values)
"""
