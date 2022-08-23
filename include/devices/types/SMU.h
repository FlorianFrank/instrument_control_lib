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

    enum AUTOZERO
    {
        /** AutoZero Disabled **/
        OFF,
        /** Perform AutoZero once and disable afterwards. **/
        ONCE,
        /** Automatic check the reference and zero measurements. An AutoZero is performed when needed. **/
        AUTO
    };

    enum SRC_FUNC {
        /** Current source function **/
        DC_AMPS,
        /** Voltage source function **/
        DC_VOLTS
    };

    enum SRC_OFF_MODE {
        /** Configures the source function according to source.offfunc. Currently not supported! **/
        OUTPUT_NORMAL,
        /** Set source output to 0V. **/
        OUTPUT_ZERO,
        /** Open source output relay. **/
        OUTPUT_HIGH_Z
    };

    explicit SMU(const char *ip, int timeoutInMs, PIL::Logging *logger);

    virtual PIL_ERROR_CODE measure(UNIT unit, SMU_CHANNEL channel, double* value) = 0;

    virtual PIL_ERROR_CODE turnOn(SMU_CHANNEL channel) = 0;
    virtual PIL_ERROR_CODE turnOff(SMU_CHANNEL channel) = 0;

    virtual PIL_ERROR_CODE setLimit(UNIT unit, SMU_CHANNEL channel, double limit) = 0;
    virtual PIL_ERROR_CODE setLevel(UNIT unit, SMU_CHANNEL channel, double level) = 0;
};


#endif //INSTRUMENT_CONTROL_LIB_SMU_H
