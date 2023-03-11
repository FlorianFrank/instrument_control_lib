import time

from py_instrument_control_lib import *

ip = "132.231.14.168"
timeout = 2000
checkErrorBuffer = False


def check_error_code(code):
    if code != ERROR_CODE.NO_ERROR:
        print('The last function returned an error code indicating something went wrong.')
        raise RuntimeError()


def use_smu(smu):
    error_code = smu.connect()
    check_error_code(error_code)
    error_code = smu.clearErrorBuffer()
    check_error_code(error_code)
    print(f'getDeviceIdentifier returned {smu.getDeviceIdentifier()}')

    error_code = smu.beep(0.2, 500, 0)
    check_error_code(error_code)

    error_code = smu.turnOff(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
    check_error_code(error_code)

    error_code = smu.setLevel(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, 0.42, checkErrorBuffer)
    check_error_code(error_code)

    error_code = smu.turnOn(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
    check_error_code(error_code)

    for i in range(10):
        error_code = smu.setLevel(SMU_UNIT.CURRENT, SMU_CHANNEL.CHANNEL_A, i * 100, checkErrorBuffer)
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
    print(f'getDeviceIdentifier returned {smu.getDeviceIdentifier()}')  # fails because we disconnected already


if __name__ == "__main__":
    smu = KEI2600(ip, timeout, SEND_METHOD.DIRECT_SEND)
    try:
        use_smu(smu)
    except RuntimeError:
        print('Graceful exit.')
