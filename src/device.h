#ifndef CE_DEVICE_DEVICE_H
#define CE_DEVICE_DEVICE_H

#include <string>
//#include "spd1305.h"

class Device {

public:
    explicit Device(const char *ip);

    int connect();

    int exec(std::string message, char *result = nullptr, bool = true, int size = 1024);

    int exec_commands(std::string &commands);

    void what_am_i();

    void cli();

protected:
    const char *ip;
    std::string name{};
    int sockfd = 0;
    int port = 5025;

};

#endif //CE_DEVICE_DEVICE_H
