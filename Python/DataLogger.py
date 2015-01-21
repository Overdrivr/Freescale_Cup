# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from collections import deque
import struct
from pubsub import pub
import os as os
import io as io
import datetime

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
            self.records[varid] = deque(maxlen=self.limit)

        self.records[varid].append(data)

    def record_all(self):
        folderpath = os.path.dirname(os.path.realpath(__file__)) + os.path.normpath("/Datalogging/")
        if not os.path.exists(folderpath):
            os.makedirs(folderpath)

        folderpath = folderpath + os.path.normpath('/' + str(datetime.datetime.now().strftime("%Y_%m_%d_%H-%M-%S/")))
        os.makedirs(folderpath)
        
        for key, queue in self.records.items():
            print("Recording variable ",key,"------")
            f = io.open(os.path.normpath(folderpath + '/' + str(key) + '.txt'),'w')
            while queue:
                 data = queue.popleft()
                 f.write(str(data['time']) + "\t" + str(data['values'][0])+ '\n')
            f.close()
            
