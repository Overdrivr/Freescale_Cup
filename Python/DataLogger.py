# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from collections import deque
import struct
from pubsub import pub
import os as os
import io as io
import datetime
import zipfile as zf

try:
    import zlib
    compression = zf.ZIP_DEFLATED
except:
    compression = zf.ZIP_STORED

modes = { zf.ZIP_DEFLATED: 'deflated',
          zf.ZIP_STORED:   'stored',
          }


# DataLogger : To record to a file n last values of MCU variables

class DataLogger():
    def __init__(self):
        self.records = dict()
        self.table = dict()
        self.limit = 3000
        pub.subscribe(self.new_data,'var_value_update')
        pub.subscribe(self.new_data_table,'logtable_update')

    def queue_size(self,queuesize):
        self.records = dict()
        self.limit = queuesize

    def new_data(self,varid,data):
        if not varid in self.records:
            self.records[varid] = deque(maxlen=self.limit)

        self.records[varid].append(data)

    def new_data_table(self,varlist):
        self.table = varlist

    def start(self):
        self.records = dict()

    def record_all(self):
        folderpath = os.path.dirname(os.path.realpath(__file__)) + os.path.normpath("/Datalogging/")
        if not os.path.exists(folderpath):
            os.makedirs(folderpath)

        filename = folderpath + os.path.normpath('/' + str(datetime.datetime.now().strftime("%Y_%m_%d_%H-%M-%S.zip")))
        print("Recording to ",folderpath)
        archive = zf.ZipFile(filename,'w')
        
        
        for key, queue in self.records.items():
            print("Recording variable ",key,"------")
            string = ""
            # Write header
            if self.table:
                string += str(key) + '\n'
                string += self.table[key]['name'] + '\n'
                string += str(self.table[key]['datatype']) + '\n'
                string += str(self.table[key]['amount']) + '\n'
                    
            while queue:
                data = queue.popleft()                
                # Write data to file
                string += str(data['time'])
                for i in range(len(data['values'])):
                    string += "\t" + str(data['values'][i]) 
                string += '\n'
                 
            archive.writestr(str(key) + '.txt',string, compress_type=compression)

        archive.close()
        print("Recording done.")
