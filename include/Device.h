/**
 * @brief Base class containing the base functionality of all devices.
 * And parameters to store the name connection type, etc.
 * @authors Wuhao Liu, Florian Frank
 * @copyright University of Passau
 */
#ifndef CE_DEVICE_DEVICE_H
#define CE_DEVICE_DEVICE_H

extern "C"
{
#include "ctlib/ErrorCodeDefines.h"
}

#include "ExecArgs.h"

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
    explicit Device(const char *ip, int timeoutInMs);
    explicit Device(const char *ip, int timeoutInMs, PIL::Logging *logger);
    ~Device();

    PIL_ERROR_CODE Connect();
    PIL_ERROR_CODE Disconnect();
    [[nodiscard]] bool IsOpen() const;

    std::string GetDeviceIdentifier();
    std::string WhatAmI();

    PIL_ERROR_CODE Exec(std::string command, ExecArgs *args = nullptr, char *result = nullptr, bool br = true, int size = 1024);
    PIL_ERROR_CODE ExecCommands(std::string &commands);

    std::string ReturnErrorMessage();

protected:
    std::string m_IPAddr;
    PIL_ErrorHandle m_ErrorHandle;
    std::string m_DeviceName{};
    PIL::Socket *m_SocketHandle;
    PIL::Logging *m_Logger;
    int m_Port = 5025;
};

#endif //CE_DEVICE_DEVICE_H
