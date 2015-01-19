# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from queue import Queue
import struct
from pubsub import pub

# DataLogger : To record to a file n last values of MCU variables

class DataLogger():
    def __init__(self):
        self.records = dict()
        self.limit = 1000
        pub.subscribe(self.new_data,'var_value_update')

    def queue_size(self,queuesize):
        self.records = dict()
        self.limit = queuesize

    def new_data(self,varid,data):
        if not varid in self.records:
            self.records[varid] = Queue(self.limit)

        self.records[varid].put(data)

    def record_all(self):
        for key, queue in self.records.items():
            print("Variable ",key,"------")
            while not queue.empty():
                 data = queue.get()
                 print(data['time']," : ",data['values'])
            
            
