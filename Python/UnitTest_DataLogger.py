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

logger = DataLogger()
time = 0

while time < 1:
    fake_value_decoded(0,time,list([math.cos(time),]))
    fake_value_decoded(1,time,list([math.sin(time),]))
    l = list()
    l.append(math.tan(time))
    l.append(math.atan(time))
    fake_value_decoded(2,time,l)
    time += 0.01

logger.record_all()
print("Done.")

    

