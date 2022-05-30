/**
 * @brief Base class containing the base functionality of all devices.
 * And parameters to store the name connection type, etc.
 * @authors Wuhao Liu, Florian Frank
 * @copyright University of Passau
 */
#ifndef CE_DEVICE_DEVICE_H
#define CE_DEVICE_DEVICE_H

#include "ErrorHandler.h"

#include <string>


class Device {

public:
    explicit Device(const char *ip);
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
    const char *m_IPAddr;
    std::string m_DeviceName{};
    int m_SocketHandle = 0;
    int m_Port = 5025;
    bool m_IsOpen;
};

#endif //CE_DEVICE_DEVICE_H
