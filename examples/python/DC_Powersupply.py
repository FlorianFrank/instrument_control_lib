from libpy_icl import *

def test_spd():
    spd = SPD1305("<ip-address>", 0)
    spd.Connect() # TODO check return code
    current = spd.getCurrent(DC_CHANNEL.CHANNEL_1)
    spd.Disconnect()

if __name__ == '__main__':
    test_spd()