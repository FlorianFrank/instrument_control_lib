"""
>>> from libpy_icl import *
>>> spd = SPD1305("132.231.14.176", 0)
>>> spd.Connect()
True
>>> spd.setCurrent(DC_CHANNEL.CHANNEL_1, 0.1)
0
>>> current = spd.getCurrent(DC_CHANNEL.CHANNEL_1)
>>> current == 0.1
True
>>> spd.Disconnect()
True
"""
from libpy_icl import *

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

