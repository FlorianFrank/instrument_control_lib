from py_instrument_control_lib import *
from time import sleep

ip = "132.231.14.168"
timeout = 2000


def handle_error_code(code, command):
    print(f'{command: <15} executed with error code {code}')


def test_smu(smu):
    handle_error_code(smu.connect(), "connect")
    handle_error_code(smu.clearErrorBuffer(), "clearErrorBuffer")
    print(f'getDeviceIdentifier returned {smu.getDeviceIdentifier()}')

    handle_error_code(smu.enableBeep(1), "enableBeep")
    # handle_error_code(smu.beep(0.2, 500, 0), "beep")
    handle_error_code(smu.disableBeep(0), "disableBeep")

    handle_error_code(smu.turnOn(SMU_CHANNEL.CHANNEL_A, 1), "turnOn A")
    handle_error_code(smu.turnOff(SMU_CHANNEL.CHANNEL_A, 0), "turnOff A")

    handle_error_code(smu.turnOn(SMU_CHANNEL.CHANNEL_B, 1), "turnOn B")
    handle_error_code(smu.turnOff(SMU_CHANNEL.CHANNEL_B, 0), "turnOff B")

    print(f'SMU measured {smu.measure(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, 1)} V on CHANNEL_A')

    print(f'Last error: {smu.getLastError()}')
    print(f'getErrorBufferStatus returned {smu.getErrorBufferStatus()}')
    handle_error_code(smu.clearErrorBuffer(), "clearErrorBuffer")

    handle_error_code(smu.disconnect(), "disconnect")
    print(f'getDeviceIdentifier returned {smu.getDeviceIdentifier()}')


if __name__ == "__main__":
    smu = KEI2600(ip, timeout)
    test_smu(smu)
