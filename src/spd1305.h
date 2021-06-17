//
// Created by liuwuhao on 20.05.21.
//

#ifndef CE_DEVICE_SPD1305_H
#define CE_DEVICE_SPD1305_H


#include "device.h"

using namespace std;

class SPD1305 : public Device {
    int sockfd = 0;
public:
    SPD1305(char* ip);

    int set_current(double current, int = 1);
    double get_current(int = 1);
    int turnon(int = 1);
    int turnoff(int = 1);
};

#endif //CE_DEVICE_SPD1305_H
