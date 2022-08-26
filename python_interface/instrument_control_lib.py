
from py_instrument_control_lib import *
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

def smu_test():
	smu = KEI2600("192.168.1.10", 2000)
	errorCode = smu.Connect()
	if errorCode != ERROR_CODE.NO_ERROR:
		print(errorCode)
	print(smu.GetDeviceIdentifier())
	errorCode = smu.turnOn(SMU_CHANNEL.CHANNEL_A)
	if errorCode != ERROR_CODE.NO_ERROR:
		print(errorCode)

if __name__ == "__main__":
	smu_test()

