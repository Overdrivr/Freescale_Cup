import matplotlib
matplotlib.use('TkAgg')

from numpy import arange, sin, pi
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg

import tkinter as Tk
import random
import sys
from threading import Thread
import time

from matplotlib.figure import Figure
from SerialPortHandler import SerialPortHandler
from GUI import Application


app = Application()
app.mainloop()


