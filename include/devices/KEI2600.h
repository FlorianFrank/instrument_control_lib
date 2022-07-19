//
// Created by sueaki on 03.06.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_KEI2600_H
#define INSTRUMENT_CONTROL_LIB_KEI2600_H

#include "Device.h"
#include "types/SMU.h"

namespace PIL {
    class Logging;
}

class KEI2600 : public SMU {
public:

    explicit KEI2600(PIL::Logging *logger, const char *ip, int timeoutInMS);
    KEI2600(const char *ip, int timeoutInMs, PIL::Logging *logger);

    double measure(UNIT unit, SMU_CHANNEL channel) override;

    int turnOn(SMU_CHANNEL channel) override;
    int turnOff(SMU_CHANNEL channel) override;

    int setLevel(UNIT unit, SMU_CHANNEL channel, double level) override;
    int setLimit(UNIT unit, SMU_CHANNEL channel, double limit) override;

    int enableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel);
    int disableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel);

    int enableSourceAutoRange(UNIT unit, SMU_CHANNEL channel);
    int disableSourceAutoRange(UNIT unit, SMU_CHANNEL channel);

    int setMeasureRange(UNIT unit, SMU_CHANNEL channel, double range);
    int setSourceRange(UNIT unit, SMU_CHANNEL channel, double range);

    int selectLocalSense(SMU_CHANNEL channel);
    int selectRemoteSense(SMU_CHANNEL channel);

    int enableBeep();
    int beep();
    int disableBeep();

private:
    double measureI(SMU_CHANNEL channel);
    double measureV(SMU_CHANNEL channel);
    double measureR(SMU_CHANNEL channel);
    double measureP(SMU_CHANNEL channel);



};

#endif //INSTRUMENT_CONTROL_LIB_KEI2600_H
