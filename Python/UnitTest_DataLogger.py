# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from DataLogger import DataLogger
from pubsub import pub
from threading import Timer
import struct as struct
import time
import math as math

def fake_value_decoded(varid,t,value):
    d = dict()
    d['time'] = t
    d['values'] = value
    pub.sendMessage('var_value_update',varid=varid,data=d)

def fake_table_decoded():
    v = dict()

    v[0] = dict()
    v[0]['datatype'] = 0
    v[0]['octets'] = 4
    v[0]['is_array'] = False
    v[0]['amount'] = 1
    v[0]['writeable'] = False
    v[0]['name'] = "cos(x)"

    v[1] = dict()
    v[1]['datatype'] = 0
    v[1]['octets'] = 4
    v[1]['is_array'] = False
    v[1]['amount'] = 1
    v[1]['writeable'] = True
    v[1]['name'] = "sin(x)"

    v[2] = dict()
    v[2]['datatype'] = 0
    v[2]['octets'] = 8
    v[2]['is_array'] = True
    v[2]['amount'] = 2
    v[2]['writeable'] = False
    v[2]['name'] = "tan(x) atan(x)"

    pub.sendMessage('logtable_update',varlist=v)

logger = DataLogger()
time = 0

fake_table_decoded()

while time < 10.0:
    fake_value_decoded(0,time,list([math.cos(time),]))
    fake_value_decoded(1,time,list([math.sin(time),]))
    l = list()
    l.append(math.tan(time))
    l.append(math.atan(time))
    fake_value_decoded(2,time,l)
    time += 0.01
    #print(time)
    
print("Recording")
logger.record_all()
print("Done.")

    

