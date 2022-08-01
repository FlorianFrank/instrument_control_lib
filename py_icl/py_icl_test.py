
from libpy_icl import *
import os
import sys

os.add_dll_directory(os.getcwd())


def spd_test():
	spd = SPD1305("132.231.14.176", 0)
	spd.Connect()
	spd.setCurrent(DC_CHANNEL.CHANNEL_1, 0.1)
	current = spd.getCurrent(DC_CHANNEL.CHANNEL_1)
	current == 0.1
	spd.Disconnect()


if __name__ == "__main__":
    import doctest
    doctest.testmod()

