#ifndef CE_DEVICE_DEVICE_H
#define CE_DEVICE_DEVICE_H

#include <string>
//#include "spd1305.h"

using namespace std;

class Device {
    string name = "Device";
    int sockfd = 0;

protected:
    char* ip;

public:
    int port = 5025;
    int test();
    virtual int connect();
    virtual int exec(string message, char *result = nullptr, bool = true);
    Device(char* ip, unsigned short port);
    Device(char* ip);
};

#endif //CE_DEVICE_DEVICE_H
