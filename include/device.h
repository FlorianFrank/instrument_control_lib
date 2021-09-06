#ifndef CE_DEVICE_DEVICE_H
#define CE_DEVICE_DEVICE_H

#include <string>
#include "error_handler.h"
//#include "spd1305.h"

class Device {

public:
    explicit Device(const char *ip);
    ~Device();

    bool connect();

    bool disconnect();

    bool isOpen();

    std::string getDeviceIdentifier();

    bool exec(std::string message, char *result = nullptr, bool br = true, int size = 1024);

    bool exec_commands(std::string &commands);

    std::string what_am_i();

    PIL_ErrorHandle m_ErrorHandle;

    std::string return_error_message();

protected:
    const char *m_IPAddr;
    std::string m_DeviceName{};
    int m_SocketHandle = 0;
    int m_Port = 5025;
    bool m_IsOpen;
};

#endif //CE_DEVICE_DEVICE_H
