//
// Created by liuwuhao on 20.05.21.
//

#ifndef CE_DEVICE_SPD1305_H
#define CE_DEVICE_SPD1305_H


#include "types/DCPowerSupply.h"

namespace PIL {
    class Logging;
}

class SPD1305 : public DCPowerSupply {

public:
    explicit SPD1305(const char *ip, int timeoutInMS);

    explicit SPD1305(const char *ip, PIL::Logging *logger, int timeoutInMs);

    PIL_ERROR_CODE turnOn(DC_CHANNEL channel) override;
    PIL_ERROR_CODE turnOff(DC_CHANNEL channel) override;

    PIL_ERROR_CODE setCurrent(DC_CHANNEL channel, double current) override;
    PIL_ERROR_CODE getCurrent(DC_CHANNEL channel, double* current) override;
    // TODO add missing functions
private:
    static std::string getStrFromDCChannelEnum(DC_CHANNEL channel);
};

#endif //CE_DEVICE_SPD1305_H
