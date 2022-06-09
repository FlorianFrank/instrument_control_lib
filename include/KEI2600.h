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
    explicit KEI2600(const char *ip, int timeoutInMS);

    KEI2600(const char *ip, int timeoutInMs, PIL::Logging *logger);

    int enableBeep();

    int beep();

    int disableBeep();

    int enableMeasureAutorangeI(char channel = 'a');

    int enableSourceAutorangeI(char channel = 'a');

    int disableMeasureAutorangeI(char channel = 'a');

    int disableSourceAutorangeI(char channel = 'a');

    int enableMeasureAutorangeV(char channel = 'a');

    int enableSourceAutorangeV(char channel = 'a');

    int disableMeasureAutorangeV(char channel = 'a');

    int disableSourceAutorangeV(char channel = 'a');

    int setMeasureRangeI(double, char channel = 'a');

    int setSourceRangeI(double, char channel = 'a');

    int setMeasureRangeV(double, char channel = 'a');

    int setSourceRangeV(double, char channel = 'a');

    double readI(char channel = 'a');

    double readV(char channel = 'a');

    double readR(char channel = 'a');

    double readP(char channel = 'a');

    int setLevelI(double, char channel = 'a');

    int setLevelV(double, char channel = 'a');

    int setLimitI(double, char channel = 'a');

    int setLimitV(double, char channel = 'a');

    int setLimitP(double, char channel = 'a');

    int turnOn(char channel = 'a');

    int turnOff(char channel = 'a');

    int selectLocalSense(char channel = 'a');

    int selectRemoteSense(char channel = 'a');
};

#endif //INSTRUMENT_CONTROL_LIB_KEI2600_H
