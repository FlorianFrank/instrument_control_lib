/**
 * @brief Base class containing the base functionality of all devices.
 * And parameters to store the name connection type, etc.
 * @authors Wuhao Liu, Florian Frank
 * @copyright University of Passau
 */
#ifndef CE_DEVICE_DEVICE_H
#define CE_DEVICE_DEVICE_H

extern "C"{
#include "ctlib/ErrorHandler.h"
}

#include "ExecArgs.h"
#include "ctlib/Logging.hpp"

#include <string>
#include <vector>

namespace PIL
{
    class Socket;
    class Logging;
}

/**
 * @class Device
 * @brief Basic device class
 * This class contains all basic method of all devices, like Connect, Exec, WhatAmI etc.
 */
class Device {

public:
    explicit Device(std::string ipAddress, int timeoutInMs);
    explicit Device(std::string ipAddress, int timeoutInMs, PIL::Logging *logger);
    ~Device();

    PIL_ERROR_CODE Connect();
    PIL_ERROR_CODE Disconnect();
    [[nodiscard]] bool IsOpen() const;

    std::string GetDeviceIdentifier();

    PIL_ERROR_CODE Exec(std::string command, ExecArgs *args = nullptr, char *result = nullptr, bool br = true, int size = 1024);
    PIL_ERROR_CODE ExecCommands(std::string &commands);

    std::string ReturnErrorMessage();

protected:
    PIL_ERROR_CODE
    handleErrorsAndLogging(PIL_ERROR_CODE errorCode, bool throwException, PIL::Level logLevel, const char *fileName,
                           int line, const char* formatStr, ...);


    std::string m_IPAddr;
    PIL_ErrorHandle m_ErrorHandle;
    std::string m_DeviceName{};
    PIL::Socket *m_SocketHandle;
    PIL::Logging *m_Logger;
    uint8_t m_recvBuff[2048]{};
    int m_Port = 5025;
};

#endif //CE_DEVICE_DEVICE_H
