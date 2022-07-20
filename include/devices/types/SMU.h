//
// Created by florianfrank on 19.07.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_SMU_H
#define INSTRUMENT_CONTROL_LIB_SMU_H
#include "Device.h"
/**
 * @brief Dummy class to allow assignment to SMU class.
 */
class SMU : public Device
{

public:

    /**
* @brief Used to select which channel of the SMU should be used to measure or supplied.
*/
    enum SMU_CHANNEL
    {
        CHANNEL_A = 'a',
        CHANNEL_B = 'b'
    };

    /**
     * @brief Unit which should be changed or measured.
     */
    enum UNIT
    {
        VOLTAGE = 0,
        CURRENT = 1,
        RESISTANCE = 2,
        POWER = 3
    };

    explicit SMU(const char *ip, int timeoutInMs, PIL::Logging *logger);

    virtual double measure(UNIT unit, SMU_CHANNEL channel) = 0;

    virtual int turnOn(SMU_CHANNEL channel) = 0;
    virtual int turnOff(SMU_CHANNEL channel) = 0;

    virtual int setLimit(UNIT unit, SMU_CHANNEL channel, double limit) = 0;
    virtual int setLevel(UNIT unit, SMU_CHANNEL channel, double level) = 0;
};


#endif //INSTRUMENT_CONTROL_LIB_SMU_H
