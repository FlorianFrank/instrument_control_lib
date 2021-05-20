//
// Created by liuwuhao on 20.05.21.
//

#ifndef CE_DEVICE_SPD1305_H
#define CE_DEVICE_SPD1305_H


#include "library.h"

using namespace std;

class SPD1305 : public Device {
    string name = "DC Power Supply";
    int sockfd = 0;
protected:
    unsigned short port = 5025;

public:
    SPD1305(char* ip);
    int connect();
    int exec(char* message, char* = nullptr);

    int setCurrent(double current, int = 1);
    double getCurrent(int = 1);
};

#endif //CE_DEVICE_SPD1305_H
