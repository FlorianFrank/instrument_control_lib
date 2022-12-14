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

#define DEVICE_NAME           "Keithley SMU 2600B"

/** Buffer size when returning values from measureI, measureV or measureP*/
#define MEASURE_RET_BUFF_SIZE 1024

/**
 * @brief This class implements the basic functionality of Keithley 2600 series SMU's.
 */
class KEI2600 : public SMU
{
public:
    explicit KEI2600(const char *ip, int timeoutInMs, PIL::Logging *logger, SEND_METHOD mode);

    virtual ~KEI2600(){};

    [[maybe_unused]] explicit KEI2600(const char *ip, int timeoutInMs, SEND_METHOD mode);

    PIL_ERROR_CODE measure(UNIT unit, SMU_CHANNEL channel, double *value, bool checkErrorBuffer) override;
    double measurePy(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer);

    PIL_ERROR_CODE turnOn(SMU_CHANNEL channel, bool checkErrorBuffer) override;
    PIL_ERROR_CODE turnOff(SMU_CHANNEL channel, bool checkErrorBuffer) override;

    PIL_ERROR_CODE setLevel(UNIT unit, SMU_CHANNEL channel, double level, bool checkErrorBuffer) override;
    PIL_ERROR_CODE setLimit(UNIT unit, SMU_CHANNEL channel, double limit, bool checkErrorBuffer) override;

    PIL_ERROR_CODE enableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer);
    PIL_ERROR_CODE disableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer);

    PIL_ERROR_CODE enableSourceAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer);
    PIL_ERROR_CODE disableSourceAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer);

    [[maybe_unused]] PIL_ERROR_CODE enableMeasureAnalogFilter(SMU_CHANNEL channel, bool checkErrorBuffer);
    [[maybe_unused]] PIL_ERROR_CODE disableMeasureAnalogFilter(SMU_CHANNEL channel, bool checkErrorBuffer);

    PIL_ERROR_CODE setMeasureRange(UNIT unit, SMU_CHANNEL channel, double range, bool checkErrorBuffer);
    PIL_ERROR_CODE setSourceRange(UNIT unit, SMU_CHANNEL channel, double range, bool checkErrorBuffer);

    PIL_ERROR_CODE setSenseMode(SMU_CHANNEL channel, SMU_SENSE senseArg, bool checkErrorBuffer);
    PIL_ERROR_CODE setMeasurePLC(SMU_CHANNEL channel, double value, bool checkErrorBuffer);
    PIL_ERROR_CODE setMeasureLowRange(UNIT unit, SMU_CHANNEL channel, double value, bool checkErrorBuffer);
    PIL_ERROR_CODE setMeasureAutoZero(SMU_CHANNEL channel, AUTOZERO autoZero, bool checkErrorBuffer);
    PIL_ERROR_CODE setMeasureCount(SMU_CHANNEL channel, int nrOfMeasurements, bool checkErrorBuffer);
    PIL_ERROR_CODE setSourceFunction(SMU_CHANNEL channel, SRC_FUNC srcFunc, bool checkErrorBuffer);
    PIL_ERROR_CODE setSourceOffMode(SMU_CHANNEL channel, SRC_OFF_MODE srcOffMode, bool checkErrorBuffer);
    PIL_ERROR_CODE setSourceSettling(SMU_CHANNEL channel, SRC_SETTLING srcSettling, bool checkErrorBuffer);
    PIL_ERROR_CODE enableSourceSink(SMU_CHANNEL channel, bool checkErrorBuffer);
    PIL_ERROR_CODE disableSourceSink(SMU_CHANNEL channel, bool checkErrorBuffer);
    PIL_ERROR_CODE displayMeasureFunction(SMU_CHANNEL channel, SMU_DISPLAY displayMeasureFunc, bool checkErrorBuffer);

    PIL_ERROR_CODE enableBeep(bool checkErrorBuffer = true);
    PIL_ERROR_CODE beep(float timeInSeconds, int frequency, bool checkErrorBuffer);
    PIL_ERROR_CODE disableBeep(bool checkErrorBuffer);

    std::string getLastError();
    PIL_ERROR_CODE clearErrorBuffer();
    PIL_ERROR_CODE getErrorBufferStatus();

    PIL_ERROR_CODE performLinearVoltageSweep(SMU_CHANNEL channel, double startVoltage, double stopVoltage,
                                             int increaseRate, double current, bool checkErrorBuffer);
    PIL_ERROR_CODE sendScript(std::string script, std::string scriptName, bool checkErrorBuffer);
    PIL_ERROR_CODE executeScript(std::string scriptName, bool checkErrorBuffer);
    PIL_ERROR_CODE sendAndExecuteScript(std::string script, std::string scriptName, bool checkErrorBuffer);
    PIL_ERROR_CODE executeBufferedScript(bool checkErrorBuffer);

private:
    PIL_ERROR_CODE measureI(SMU_CHANNEL channel, double *value);
    PIL_ERROR_CODE measureV(SMU_CHANNEL channel, double *value);
    PIL_ERROR_CODE measureR(SMU_CHANNEL channel, double *value);
    PIL_ERROR_CODE measureP(SMU_CHANNEL channel, double *value);

    PIL_ERROR_CODE analogFilterHelperFunction(SMU_CHANNEL channel, bool enable);
    PIL_ERROR_CODE measureAutoRangeHelperFunction(SMU_CHANNEL channel, UNIT unit, bool enable);
    PIL_ERROR_CODE enableDisableBeepHelperFunction(bool enable);

    static std::string getChannelStringFromEnum(SMU_CHANNEL channel);
    static std::string getStringFromAutoZeroEnum(AUTOZERO autoZero);
    static std::string getStringFromSrcFuncEnum(SRC_FUNC srcFunc);
    static std::string getStringFromOffModeEnum(SRC_OFF_MODE offMode);
    static std::string getStringFromSettleEnum(SRC_SETTLING srcSettling);
    static std::string getStringFromMeasureDisplayFunction(SMU_DISPLAY displayMeasureFunc);
    static std::string getStringFromSenseValue(SMU_SENSE sense);
};