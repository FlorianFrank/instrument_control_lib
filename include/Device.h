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

    enum SEND_METHOD {
        DIREKT_SEND = 0,
        BUFFER_ENABLED = 1
    };

    explicit Device(const char *ip, int timeoutInMs, SEND_METHOD mode);
    explicit Device(const char *ip, int timeoutInMs, PIL::Logging *logger, SEND_METHOD mode);
    ~Device();

    PIL_ERROR_CODE Connect();
    PIL_ERROR_CODE Disconnect();
    [[nodiscard]] bool IsOpen() const;
    [[nodiscard]] bool IsBuffered() const;

    std::string GetDeviceIdentifier();
    std::string WhatAmI();
    PIL_ERROR_CODE delay(double delayTime);

    PIL_ERROR_CODE Exec(std::string command, ExecArgs *args = nullptr, char *result = nullptr, bool br = true, int size = 1024);
    PIL_ERROR_CODE ExecCommands(std::string &commands);

    std::string ReturnErrorMessage();

    std::string getBufferedScript();

    bool errorOccured(PIL_ERROR_CODE errorCode);

protected:
    std::string m_IPAddr;
    PIL_ErrorHandle m_ErrorHandle;
    std::string m_DeviceName{};
    PIL::Socket *m_SocketHandle;
    PIL::Logging *m_Logger;
    uint8_t m_recvBuff[2048];
    int m_Port = 5025;
    SEND_METHOD m_SendMode;
    std::string m_BufferedScript;
};

#endif //CE_DEVICE_DEVICE_H
