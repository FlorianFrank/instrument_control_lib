/**
 * @brief This file contains all functions to communicate with Keithley 2600 series SMU's.
 * @author Wuhao Liu, Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#pragma once

#include "Device.h"
#include "types/SMU.h"

namespace PIL
{
    class Logging;
}

#define DEVICE_NAME "Keithley SMU 2600B"

/**
 * @brief This class implements the basic functionality of Keithley 2600 series SMU's.
 */
class KEI2600 : public SMU
{
public:
    explicit KEI2600(const char *ip, int timeoutInMs, PIL::Logging *logger);

    PIL_ERROR_CODE measure(UNIT unit, SMU_CHANNEL channel, double* value) override;

    PIL_ERROR_CODE turnOn(SMU_CHANNEL channel) override;
    PIL_ERROR_CODE turnOff(SMU_CHANNEL channel) override;

    PIL_ERROR_CODE setLevel(UNIT unit, SMU_CHANNEL channel, double level) override;
    PIL_ERROR_CODE setLimit(UNIT unit, SMU_CHANNEL channel, double limit) override;

    PIL_ERROR_CODE enableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel);
    PIL_ERROR_CODE disableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel);

    PIL_ERROR_CODE enableSourceAutoRange(UNIT unit, SMU_CHANNEL channel);
    PIL_ERROR_CODE disableSourceAutoRange(UNIT unit, SMU_CHANNEL channel);

    PIL_ERROR_CODE setMeasureRange(UNIT unit, SMU_CHANNEL channel, double range);
    PIL_ERROR_CODE setSourceRange(UNIT unit, SMU_CHANNEL channel, double range);

    PIL_ERROR_CODE selectLocalSense(SMU_CHANNEL channel);
    PIL_ERROR_CODE selectRemoteSense(SMU_CHANNEL channel);

    PIL_ERROR_CODE enableBeep();
    PIL_ERROR_CODE beep();
    PIL_ERROR_CODE disableBeep();

private:
    PIL_ERROR_CODE measureI(SMU_CHANNEL channel, double *value);
    PIL_ERROR_CODE measureV(SMU_CHANNEL channel, double *value);
    PIL_ERROR_CODE measureR(SMU_CHANNEL channel, double *value);
    PIL_ERROR_CODE measureP(SMU_CHANNEL channel, double *value);

    static std::string getChannelStringFromEnum(SMU_CHANNEL channel);
};