#ifndef CE_DEVICE_DEVICE_H
#define CE_DEVICE_DEVICE_H

#include <string>
//#include "spd1305.h"

using namespace std;

class Device {
protected:
    const char *ip;

public:
    string name{};
    int sockfd = 0;
    int port = 5025;


    int test();

    int connect();

    int exec(string message, char *result = nullptr, bool = true, int size = 1024);

    int exec_commands(string commands);

    void what_am_i();

    void cli();

    Device(const char *ip, unsigned short port);

    Device(const char *ip);
};

#endif //CE_DEVICE_DEVICE_H
