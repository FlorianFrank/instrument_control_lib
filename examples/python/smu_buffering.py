"""
Shows how to use command buffering with a Keithley SMU.
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

    # Change the send mode to direct temporarily to clear the error buffer first.
    smu.changeSendMode(SEND_METHOD.DIRECT_SEND)
    error_code = smu.clearErrorBuffer()
    check_error_code(error_code)

    error_code = smu.disableBeep(False)
    check_error_code(error_code)
    smu.changeSendMode(SEND_METHOD.BUFFER_ENABLED)

    # These commands are all not executed directly, but saved and executed in batch when executing the buffered script.
    smu.turnOff(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
    smu.setLevel(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, 0.42, checkErrorBuffer)
    smu.turnOn(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)

    for i in range(10):
        smu.setLevel(SMU_UNIT.CURRENT, SMU_CHANNEL.CHANNEL_A, i / 10, checkErrorBuffer)
        smu.measure(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
        smu.measure(SMU_UNIT.CURRENT, SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
        smu.delay(0.1)  # When buffering, use smu.delay for the delay to be in the buffered script

    smu.turnOff(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
    smu.turnOff(SMU_CHANNEL.CHANNEL_B, checkErrorBuffer)

    smu.measure(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)

    print(f'Last error: {smu.getLastError()}')

    # Send the buffered script (the commands above) to the smu and execute it.
    error_code = smu.executeBufferedScript(checkErrorBuffer)
    check_error_code(error_code)

    smu.changeSendMode(SEND_METHOD.DIRECT_SEND)
    time.sleep(5)  # Wait for script to finish running

    # If there are measurements when buffering, the values are saved in a channel buffer. They are saved in the order
    # they are measured in a list. You have to manually make sense of these values (see below). These values can be
    # retrieved after executing the buffered script.
    values_channel_a = smu.getBuffer(smu.CHANNEL_A_BUFFER, False)
    print(f'Measured the following values when executing buffered Script: {values_channel_a}')

    # There are various methods of making sense of the measured values. The simplest is to use the same program as when
    # measuring, but now retrieving the values and removing commands that do not return anything.
    # The counter ensures all values are read linearly.
    ctr = 0
    for i in range(10):
        print(f'Measured {values_channel_a[ctr]} V on CHANNEL_A')
        ctr += 1

        print(f'Measured {values_channel_a[ctr]} A on CHANNEL_A')
        ctr += 1

    error_code = smu.disconnect()
    check_error_code(error_code)


if __name__ == "__main__":
    smu = KEI2600(ip, timeout, SEND_METHOD.BUFFER_ENABLED)
    use_smu(smu)
