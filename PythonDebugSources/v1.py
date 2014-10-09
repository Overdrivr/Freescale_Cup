"""
ldr.py
 
Display analog data from Arduino using Python (matplotlib)
 
Author: Mahesh Venkitachalam
Website: electronut.in
"""
 
import sys, serial, argparse
import numpy as np
from time import sleep
from collections import deque
 
import matplotlib.pyplot as plt 
import matplotlib.animation as animation
 
    
# main() function
def main():

  #Start connection
  strPort = 'COM5'
  ser = serial.Serial(strPort, 115200)
  print('reading from serial port %s...' % strPort)
     
  # set up animation
  fig = plt.figure()
  ax = plt.axes(xlim=(0, 127), ylim=(-20, 5000))
  a0, = ax.plot([], [])
   
  def init():
    print('init')
    data = np.arange(128)
    data.fill(0)
    a0.set_data(np.arange(128),data)
    return a0,
      
  # update
  def updated(i):
       serialout = ""
       serialout = ser.readline()
       temp = np.fromstring(serialout, dtype=int, sep=' ')
       data = np.arange(128)
       data.fill(0)
       if temp.size == 128:
         data = temp
       #print(data)
       a0.set_data(np.arange(data.size),data)
       return a0,

  
  anim = animation.FuncAnimation(fig,updated,np.arange(1, 200),interval=10,init_func=init,blit=True)
 
  # show plot
  plt.show()
  
  # clean up
  ser.flush()
  ser.close()
 
  print('exiting.')
  
 
# call main
if __name__ == '__main__':
  main()
