# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from Model import Model
from pubsub import pub
from SerialPort import SerialPort
from threading import Timer
import struct as struct
import time
from VariableManager import VariableManager

# NOTE : Remove comments in front of prints in VariableManager.py line 89 & 94

def fake_data_received(var_id):
    d = dict()
    pub.sendMessage('var_value_update',varid=var_id,data=d)

model = Model()
mgr = VariableManager(model)
model.start()

# Todo : send a fake table
# Otherwise test fully functionnal

# Normal sequence (start 2 -> stop 2)
pub.sendMessage('using_var',varid=2)
time.sleep(1.0)
fake_data_received(2)
pub.sendMessage('stop_using_var',varid=2)

time.sleep(1.0)

# Immediate erase sequence
# (If erase happens before 500 ms
#  no start to avoid querying variables to quickly to the MCU)
pub.sendMessage('using_var',varid=2)
pub.sendMessage('stop_using_var',varid=2)

time.sleep(1.0)

# Late MCU answer sequence
pub.sendMessage('using_var',varid=3)
time.sleep(3.0)
fake_data_received(3)
pub.sendMessage('stop_using_var',varid=3)

# Multiple readers sequence
pub.sendMessage('using_var',varid=5)
time.sleep(1.0)
fake_data_received(5)
time.sleep(1.0)
pub.sendMessage('using_var',varid=5)
time.sleep(1.0)
pub.sendMessage('stop_using_var',varid=5)
time.sleep(1.0)
pub.sendMessage('stop_using_var',varid=5)
time.sleep(1.0)
model.stop()
model.join()
print("Done.")
