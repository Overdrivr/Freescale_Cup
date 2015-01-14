# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from Model import Model
from pubsub import pub
from threading import Timer

def message(rxpayload):
    print("[",model.serialthread.get_processed_octets(),",",
              model.protocol.get_processed_octets(),"]",
              model.serialthread.rxqueue.qsize()," : ",rxpayload)

def stop():
    model.stop()
    
model = Model()

#pub.subscribe(message,'new_rx_payload')

t = Timer(150.0, stop)
t.start()

model.start()
model.connect_com("COM6")

model.join()

print("Done.")
print("RX : ",model.serialthread.get_processed_octets())
print("PR : ",model.protocol.get_processed_octets())
print("NPY : ",model.serialthread.rxqueue.qsize())
