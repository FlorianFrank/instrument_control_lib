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

    smu.changeSendMode(SEND_METHOD.DIRECT_SEND)
    error_code = smu.clearErrorBuffer()
    check_error_code(error_code)
    smu.changeSendMode(SEND_METHOD.BUFFER_ENABLED)

    smu.turnOff(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)

    smu.setLevel(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, 0.42, checkErrorBuffer)

    smu.turnOn(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)

    for i in range(10):
        smu.setLevel(SMU_UNIT.CURRENT, SMU_CHANNEL.CHANNEL_A, i * 100, checkErrorBuffer)

        smu.measure(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
        smu.measure(SMU_UNIT.CURRENT, SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
        time.sleep(0.1)

    smu.turnOff(SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)
    smu.turnOff(SMU_CHANNEL.CHANNEL_B, checkErrorBuffer)

    smu.measure(SMU_UNIT.VOLTAGE, SMU_CHANNEL.CHANNEL_A, checkErrorBuffer)

    print(f'Last error: {smu.getLastError()}')
    smu.clearErrorBuffer()

    error_code = smu.executeBufferedScript(checkErrorBuffer)
    check_error_code(error_code)

    values_channel_a = smu.getBuffer(KEI2600.CHANNEL_A_BUFFER)
    print(f'Measured the following values when executing buffered Script: {values_channel_a}')

    ctr = 0
    for i in range(10):
        print(f'Measured {values_channel_a[ctr]} V on CHANNEL_A')
        ctr += 1

        print(f'Measured {values_channel_a[ctr]} A on CHANNEL_B')
        ctr += 1

    error_code = smu.disconnect()
    check_error_code(error_code)
    print(f'getDeviceIdentifier returned {smu.getDeviceIdentifier()}')  # fails because we disconnected already


if __name__ == "__main__":
    smu = KEI2600(ip, timeout, SEND_METHOD.BUFFER_ENABLED)
    try:
        use_smu(smu)
    except RuntimeError:
        print('Graceful exit.')
