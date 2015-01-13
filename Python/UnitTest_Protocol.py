# Copyright (C) 2014 Rémi Bèges
# For conditions of distribution and use, see copyright notice in the LICENSE file

from Model import Model
from pubsub import pub

def test_rx_table():
    print("table test started.")
    c = bytearray()
    #SOF
    c.append(int('f7',16))
    #CMD
    c.append(int('02',16))
    #DATATYPE
    c.append(int('07',16))
    #DATAID
    c.append(int('00',16))
    c.append(int('00',16))
    #Table
        #DATATYPE
    c.append(int('01',16))
        #DATAID
    c.append(int('00',16))
    c.append(int('00',16))
        #DATASIZE
    c.append(int('01',16))
    c.append(int('00',16))
        #NAME
    s = 'test_var                        '
    h = bytearray(s,'ascii')
    c.extend(h)

        #DATATYPE
    c.append(int('03',16))
        #DATAID
    c.append(int('01',16))
    c.append(int('00',16))
        #DATASIZE
    c.append(int('F3',16))
    c.append(int('0F',16))
        #NAME
    s = 'test_array                      '
    h = bytearray(s,'ascii')
    c.extend(h)
    
    #EOF
    c.append(int('7f',16))

    print("RX :",c)
    
    for x in c:
        t = x,
        a = bytes(t)
        pub.sendMessage('new_rx_byte',rxbyte=a)

def test_rx_value():
    print("value test started.")
    c = bytearray()
    #SOF
    c.append(int('f7',16))
    #CMD
    c.append(int('00',16))
    #DATATYPE
    c.append(int('00',16))
    #DATAID
    c.append(int('00',16))
    c.append(int('7D',16))#ESC EOF char
    c.append(int('7f',16))
    #VALUE
    c.append(int('05',16))
    c.append(int('7D',16))#ESC SOF char
    c.append(int('f7',16))
    c.append(int('E4',16))
    c.append(int('7D',16))#ESC ESC char
    c.append(int('7D',16))
    
    #EOF
    c.append(int('7f',16))

    print("RX :",c)
    
    for x in c:
        t = x,
        a = bytes(t)
        pub.sendMessage('new_rx_byte',rxbyte=a)

def print_payload(rxpayload):
    print("TX :",rxpayload)

model = Model()
pub.subscribe(print_payload,'new_rx_payload')
test_rx_table()
test_rx_value()

print("Done.")
