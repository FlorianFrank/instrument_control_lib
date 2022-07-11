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

    explicit KEI2600(PIL::Logging *logger, const char *ip, int timeoutInMS);

    KEI2600(const char *ip, int timeoutInMs, PIL::Logging *logger);

    int enableBeep();

    int beep();

    int disableBeep();

    int enableMeasureAutoRangeI(SMU_CHANNEL channel);

    int enableSourceAutoRangeI(SMU_CHANNEL channel);

    int disableMeasureAutoRangeI(SMU_CHANNEL channel);

    int disableSourceAutoRangeI(SMU_CHANNEL channel);

    int enableMeasureAutoRangeV(SMU_CHANNEL channel);

    int enableSourceAutoRangeV(SMU_CHANNEL channel);

    int disableMeasureAutoRangeV(SMU_CHANNEL channel);

    int disableSourceAutoRangeV(SMU_CHANNEL channel);

    int setMeasureRangeI(double, SMU_CHANNEL channel);

    int setSourceRangeI(double, SMU_CHANNEL channel);

    int setMeasureRangeV(double, SMU_CHANNEL channel);

    int setSourceRangeV(double, SMU_CHANNEL channel);

    double readI(SMU_CHANNEL channel);

    double readV(SMU_CHANNEL channel);

    double readR(SMU_CHANNEL channel);

    double readP(SMU_CHANNEL channel);

    int setLevelI(double, SMU_CHANNEL channel);

    int setLevelV(double, SMU_CHANNEL channel);

    int setLimitI(double, SMU_CHANNEL channel);

    int setLimitV(double, SMU_CHANNEL channel);

    int setLimitP(double, SMU_CHANNEL channel);

    int turnOn(SMU_CHANNEL channel);

    int turnOff(SMU_CHANNEL channel);

    int selectLocalSense(SMU_CHANNEL channel);

    int selectRemoteSense(SMU_CHANNEL channel);

};

#endif //INSTRUMENT_CONTROL_LIB_KEI2600_H
