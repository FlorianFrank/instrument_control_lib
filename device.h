#ifndef CE_DEVICE_DEVICE_H
#define CE_DEVICE_DEVICE_H

#include <string>
//#include "spd1305.h"

using namespace std;

class Device {
    string name;

protected:
    char* ip;
    unsigned short port;
public:
    virtual int connect();
    virtual int exec(char* message, char *result);
    Device(char* ip, unsigned short port);
};

#endif //CE_DEVICE_DEVICE_H
