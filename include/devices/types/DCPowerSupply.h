//
// Created by florianfrank on 20.07.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_DCPOWERSUPPLY_H
#define INSTRUMENT_CONTROL_LIB_DCPOWERSUPPLY_H

#include "Device.h"

class DCPowerSupply : public Device
{
public:
    enum DC_CHANNEL {
            CHANNEL_1 = 1,
            CHANNEL_2 = 2
    };

    DCPowerSupply(const char *ip, int timeoutInMs, PIL::Logging *logger);

    virtual PIL_ERROR_CODE turnOn(DC_CHANNEL channel) = 0;
    virtual PIL_ERROR_CODE turnOff(DC_CHANNEL channel) = 0;

    virtual PIL_ERROR_CODE setCurrent(DC_CHANNEL channel, double current) = 0;
    virtual PIL_ERROR_CODE  getCurrent(DC_CHANNEL channel, double* current) = 0;
};


#endif //INSTRUMENT_CONTROL_LIB_DCPOWERSUPPLY_H
