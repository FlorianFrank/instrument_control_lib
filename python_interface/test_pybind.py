from py_instrument_control_lib import *


def dc_channels():
    print(DC_CHANNEL.CHANNEL_1)
    print(DC_CHANNEL.CHANNEL_2)


def error_codes():
    print(ERROR_CODE.NO_ERROR)
    print(ERROR_CODE.PIL_INVALID_ARGUMENTS)
    print(ERROR_CODE.ERRNO)
    print(ERROR_CODE.INTERFACE_CLOSED)
    print(ERROR_CODE.INVALID_BAUDRATE)
    print(ERROR_CODE.INSUFFICIENT_RESOURCES)
    print(ERROR_CODE.INSUFFICIENT_PERMISSIONS)
    print(ERROR_CODE.DEADLOCK_DETECTED)
    print(ERROR_CODE.THREAD_NOT_JOINABLE)
    print(ERROR_CODE.THREAD_NOT_FOUND)
    print(ERROR_CODE.ONLY_PARTIALLY_READ_WRITTEN)
    print(ERROR_CODE.NO_SUCH_FILE)
    print(ERROR_CODE.UNKNOWN_ERROR)
    print(ERROR_CODE.XML_PARSING_ERROR)
    print(ERROR_CODE.PIL_ITEM_IN_ERROR_QUEUE)


def smu_channel():
    SMU_CHANNEL.CHANNEL_A
    SMU_CHANNEL.CHANNEL_B


if __name__ == "__main__":
    dc_channels()
    error_codes()
    smu_channel()
    print(KEI2600())
