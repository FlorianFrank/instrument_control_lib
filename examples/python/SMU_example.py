from libpy_icl import *


def test_SMU():
    smu = KEI2600("<ip>", 1000, 0)
    smu.Connect()
    smu.enableSourceAutoRange(SMU.Voltage, SMU.CHANNEL_A)
    smu.setLevel(SMU.Voltage, SMU.CHANNEL_A, 0.5)
    smu.turnOn(SMU.CHANNEL_A)

    measured_voltage = 0
    smumeasure(SMU.VOLTAGE, SMU.CHANNEL_A, measured_voltage);
