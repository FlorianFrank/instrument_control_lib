//
// Created by liuwuhao on 20.05.21.
//

#ifndef CE_DEVICE_SPD1305_H
#define CE_DEVICE_SPD1305_H


#include "Device.h"

namespace PIL {
    class Logging;
}

class SPD1305 : public Device {

public:
    explicit SPD1305(const char *ip, int timeoutInMS);

    explicit SPD1305(const char *ip, PIL::Logging *logger, int timeoutInMs);

    int set_current(double current, int = 1);

    double get_current(int = 1);

    int turnon(int = 1);

    int turnoff(int = 1);
};

#endif //CE_DEVICE_SPD1305_H
