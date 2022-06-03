/**
 * @brief Base class containing the base functionality of all devices.
 * And parameters to store the name connection type, etc.
 * @authors Wuhao Liu, Florian Frank
 * @copyright University of Passau
 */
#ifndef CE_DEVICE_DEVICE_H
#define CE_DEVICE_DEVICE_H

extern "C" {
#include "ctlib/ErrorCodeDefines.h"
};

#include <string>

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
    explicit Device(const char *ip, int timeoutInMs, PIL::Logging *logger);
    ~Device();

    bool Connect();

    bool Disconnect();

    bool IsOpen() const;

    std::string GetDeviceIdentifier();

    bool Exec(std::string message, char *result = nullptr, bool br = true, int size = 1024);

    bool ExecCommands(std::string &commands);

    std::string WhatAmI();

    std::string ReturnErrorMessage();

protected:
    PIL_ErrorHandle m_ErrorHandle;
    std::string m_IPAddr;
    int m_TimeoutInMs;
    std::string m_DeviceName{};
 //   int m_SocketHandle = 0;
    PIL::Socket *m_SocketHandle;
    PIL::Logging *m_Logger;
    int m_Port = 5025;
    bool m_IsOpen;

};

#endif //CE_DEVICE_DEVICE_H
