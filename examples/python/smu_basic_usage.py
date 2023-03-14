"""
Shows how to use basic commands of a Keithley SMU. The commands should be self-explanatory.
If you want to use command buffering with the SMU, see the other example `smu_buffering.py`.
"""

import time

from py_instrument_control_lib import *

ip = "132.231.14.168"
timeout = 2000
checkErrorBuffer = False


def check_error_code(code):
    """
    Checks if an error occured. If so raises an Error to exit the program.
    :param code: The error code returned by the last command.
    """
    if code != ERROR_CODE.NO_ERROR:
        print('The last function returned an error code indicating something went wrong.')
        raise RuntimeError()


def use_smu(smu):
    """
    Shows the basic usage of the lib when using a KEI2600 SMU.
    :param smu:  The smu object to perform the operations on.
    """
    error_code = smu.connect()
    check_error_code(error_code)
    error_code = smu.clearErrorBuffer()
    check_error_code(error_code)

    error_code = smu.disableBeep(False)
    check_error_code(error_code)

    error_code = smu.beep(0.2, 500, 0)
    check_error_code(error_code)

    error_code = smu.turnOff(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
    check_error_code(error_code)

    error_code = smu.setLevel(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, 0.42, checkErrorBuffer)
    check_error_code(error_code)

    error_code = smu.turnOn(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
    check_error_code(error_code)

    for i in range(10):
        error_code = smu.setLevel(SMU_UNIT.CURRENT, SMU_CHANNEL.CHANNEL_A, i / 10, checkErrorBuffer)
        check_error_code(error_code)

        print(f'Measured {smu.measure(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)} '
              f'V on CHANNEL_A')
        print(f'Measured {smu.measure(SMU_UNIT.CURRENT, SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)} '
              f'A on CHANNEL_B')
        time.sleep(0.1)

    error_code = smu.turnOff(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
    check_error_code(error_code)
    error_code = smu.turnOff(SMU_CHANNEL.CHANNEL_B, checkErrorBuffer)
    check_error_code(error_code)

    print(f'Measured {smu.measure(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)} V on CHANNEL_A')

    print(f'Last error: {smu.getLastError()}')
    print(f'getErrorBufferStatus returned {smu.getErrorBufferStatus()}')
    error_code = smu.clearErrorBuffer()
    check_error_code(error_code)

    error_code = smu.disconnect()
    check_error_code(error_code)


if __name__ == "__main__":
    smu = KEI2600(ip, timeout, SEND_METHOD.DIRECT_SEND)
    use_smu(smu)
