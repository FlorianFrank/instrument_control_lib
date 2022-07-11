//
// Created by sueaki on 03.06.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_KEI2600_H
#define INSTRUMENT_CONTROL_LIB_KEI2600_H

#include "Device.h"

namespace PIL {
    class Logging;
}

class KEI2600 : public Device {
public:
    enum SMU_CHANNEL {
        SMU_CHANNEL_A = 'a',
        SMU_CHANNEL_B = 'b'
    };

    enum UNIT {
        VOLTAGE = 0,
        CURRENT = 1,
        POWER   = 2
    };

    explicit KEI2600(PIL::Logging *logger, const char *ip, int timeoutInMS);

    KEI2600(const char *ip, int timeoutInMs, PIL::Logging *logger);

    int enableBeep();

    int beep();

    int disableBeep();

    int enableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel);

    int enableSourceAutoRange(UNIT unit, SMU_CHANNEL channel);

    int disableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel);

    int disableSourceAutoRange(UNIT unit, SMU_CHANNEL channel);

    int setMeasureRange(UNIT unit, SMU_CHANNEL channel, double range);

    int setSourceRange(UNIT unit, SMU_CHANNEL channel, double range);

    double readI(SMU_CHANNEL channel);

    double readV(SMU_CHANNEL channel);

    double readR(SMU_CHANNEL channel);

    double readP(SMU_CHANNEL channel);

    int setLevel(UNIT unit, SMU_CHANNEL channel, double level);

    int setLimit(UNIT unit, SMU_CHANNEL channel, double limit);

    int turnOn(SMU_CHANNEL channel);

    int turnOff(SMU_CHANNEL channel);

    int selectLocalSense(SMU_CHANNEL channel);

    int selectRemoteSense(SMU_CHANNEL channel);

};

#endif //INSTRUMENT_CONTROL_LIB_KEI2600_H
