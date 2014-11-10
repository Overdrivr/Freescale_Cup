from Worker import SerialWorker
from pubsub import pub

# Top-level API for the GUI
class Model():
    def __init__(self, **kwargs):        
       
        # Serial worker thread (Serial,Protocol,Logger)
        self.workerthread = SerialWorker()
        
    def get_ports(self):
        return self.workerthread.get_ports()
        
    def start_com(self,COM_port):
        if not self.workerthread.isAlive():
            self.workerthread.start()
            
        self.workerthread.start_COM(COM_port)

    def stop_com(self,COM_port):        
        self.workerthread.stop_COM()
        print('--- COM stopped.')
         
    def stop(self):
        self.workerthread.stop()

        if self.workerthread.isAlive():
            self.workerthread.join()
        print("--- All threads stopped.")
        
    def start_logger(self):
        self.workerthread.start_logger()        
        
    def stop_logger(self):
        self.workerthread.stop_logger()  
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
    * When scalar variable value has been received : 'scalar_var_update',varid,value
    * When array variable value has been received : 'array_var_update',varid,array(value)
"""
