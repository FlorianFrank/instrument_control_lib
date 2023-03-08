/**
 * @brief Base class containing the base functionality of all devices.
 * And parameters to store the name connection type, etc.
 * @authors Wuhao Liu, Florian Frank
 * @copyright University of Passau
 */
#ifndef CE_DEVICE_DEVICE_H
#define CE_DEVICE_DEVICE_H


#include "ExecArgs.h"
#include "ctlib/Logging.hpp"
#include "ctlib/Exception.h"


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
        DIRECT_SEND = 0,
        BUFFER_ENABLED = 1
    };

    explicit Device(std::string ipAddress, int timeoutInMs, SEND_METHOD mode = DIRECT_SEND, bool throwException = true);
    explicit Device(std::string ipAddress, int timeoutInMs, PIL::Logging *logger, SEND_METHOD mode = DIRECT_SEND, bool throwException = true);
    explicit Device(std::string ipAddress, uint16_t srcPort, uint16_t destPort, int timeoutInMs, PIL::Logging *logger, SEND_METHOD mode = DIRECT_SEND, bool throwException = true);
    ~Device();

    PIL_ERROR_CODE Connect();
    PIL_ERROR_CODE Disconnect();
    [[nodiscard]] bool IsOpen() const;
    [[nodiscard]] bool IsBuffered() const;

    std::string GetDeviceIdentifier();
    PIL_ERROR_CODE Exec(const std::string& command, ExecArgs *args = nullptr, char *result = nullptr, bool br = true, int size = 1024);
    PIL_ERROR_CODE Exec(const std::string &command, ExecArgs *args, std::string *result, bool br);
    PIL_ERROR_CODE ExecCommands(std::string &commands);

    std::string ReturnErrorMessage();

    std::string getBufferedScript();

    void changeSendMode(SEND_METHOD mode);

    PIL_ERROR_CODE delay(double delayTime);

protected:
    PIL_ERROR_CODE handleErrorsAndLogging(PIL_ERROR_CODE errorCode, bool throwException, PIL::Level logLevel,
                                          const std::string& fileName, int line, const std::string formatStr, ...);
    static bool errorOccured(PIL_ERROR_CODE errorCode);

    std::string m_IPAddr;
    PIL_ErrorHandle m_ErrorHandle;
    std::string m_DeviceName{};
    PIL::Socket *m_SocketHandle;
    PIL::Logging *m_Logger;
    int m_destPort = 5025;
    int m_srcPort = 5025;
    bool m_EnableExceptions;
    SEND_METHOD m_SendMode;
    std::string m_BufferedScript;
};

#endif //CE_DEVICE_DEVICE_H
