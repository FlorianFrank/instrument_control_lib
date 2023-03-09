/**
 * @brief This file contains all functions to communicate with Keithley 2600 series SMU's.
 * @author Wuhao Liu, Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#pragma once

#include "Device.h"
#include "types/SMU.h"

namespace PIL {
    class Logging;
}

/**
 * @brief This class implements the basic functionality of Keithley 2600 series SMU's.
 */
class KEI2600 : public SMU {
public:
    explicit KEI2600(std::string ipAddress, int timeoutInMs, PIL::Logging *logger, SEND_METHOD mode = DIRECT_SEND);
    [[maybe_unused]] explicit KEI2600(std::string ipAddress, int timeoutInMs, SEND_METHOD mode);

    virtual ~KEI2600() = default;

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

    PIL_ERROR_CODE enableMeasureAnalogFilter(SMU_CHANNEL channel, bool checkErrorBuffer);
    PIL_ERROR_CODE disableMeasureAnalogFilter(SMU_CHANNEL channel, bool checkErrorBuffer);

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
    PIL_ERROR_CODE disableBeep(bool checkErrorBuffer);
    PIL_ERROR_CODE beep(float timeInSeconds, int frequency, bool checkErrorBuffer);

    std::string getLastError();
    PIL_ERROR_CODE clearErrorBuffer();
    PIL_ERROR_CODE getErrorBufferStatus();

    PIL_ERROR_CODE performLinearVoltageSweep(SMU_CHANNEL channel, double startVoltage, double stopVoltage,
                                             int increaseRate, double current, bool checkErrorBuffer);

    PIL_ERROR_CODE sendScript(const std::string &scriptName, const std::string &script, bool checkErrorBuffer);
    PIL_ERROR_CODE sendVectorScript(const std::string &scriptName, const std::vector<std::string>& script,
                                    bool checkErrorBuffer);
    PIL_ERROR_CODE executeScript(const std::string &scriptName, bool checkErrorBuffer);
    PIL_ERROR_CODE sendAndExecuteScript(const std::string &scriptName, const std::string &script,
                                        bool checkErrorBuffer);
    PIL_ERROR_CODE sendAndExecuteVectorScript(const std::string &scriptName, const std::vector<std::string>& script,
                                              bool checkErrorBuffer);
    PIL_ERROR_CODE executeBufferedScript(bool checkErrorBuffer);

    PIL_ERROR_CODE readBuffer(const std::string &bufferName, std::vector<double> *result, bool checkErrorBuffer);
    std::vector<double> readBufferPy(const std::string &bufferName, bool checkErrorBuffer);
    PIL_ERROR_CODE getBufferSize(const std::string &bufferName, int *value, bool checkErrorBuffer);
    PIL_ERROR_CODE clearBuffer(const std::string &bufferName, bool checkErrorBuffer);
    void clearBufferedScript();

    std::string CHANNEL_A_BUFFER = "A_M_BUFFER";
    std::string CHANNEL_B_BUFFER = "B_M_BUFFER";

private:
    PIL_ERROR_CODE handleErrorCode(PIL_ERROR_CODE errorCode, bool checkErrorBuffer);

    PIL_ERROR_CODE toggleMeasureAnalogFilter(SMU_CHANNEL channel, bool enable);
    PIL_ERROR_CODE toggleMeasureAutoRange(SMU_CHANNEL channel, UNIT unit, bool enable);
    PIL_ERROR_CODE toggleSourceAutoRange(UNIT unit, SMU_CHANNEL channel, bool enable);
    PIL_ERROR_CODE toggleBeep(bool enable);
    PIL_ERROR_CODE toggleChannel(SMU_CHANNEL channel, bool enable);
    PIL_ERROR_CODE toggleSourceSink(SMU_CHANNEL channel, bool enable);

    std::string getMeasurementStorage(SMU_CHANNEL channel);
    PIL_ERROR_CODE readPartOfBuffer(int startIdx, int endIdx, const std::string &bufferName, char *printBuffer,
                                    std::vector<double> *result, bool checkErrorBuffer);
    PIL_ERROR_CODE appendToBuffer(int startIdx, int endIdx, const std::string &bufferName, char *printBuffer,
                                  std::vector<double> *result, bool checkErrorBuffer);

    static std::string createPayload(const std::string &value);
    static void createPayloadBatch(int offset, int numberOfLines, std::vector<std::string> values,
                                   std::vector<std::string> *result);

    static std::string getChannelStringFromEnum(SMU_CHANNEL channel);
    static std::string getStringFromAutoZeroEnum(AUTOZERO autoZero);
    static std::string getStringFromSrcFuncEnum(SRC_FUNC srcFunc);
    static std::string getStringFromOffModeEnum(SRC_OFF_MODE offMode);
    static std::string getStringFromSettleEnum(SRC_SETTLING srcSettling);
    static std::string getStringFromMeasureDisplayFunction(SMU_DISPLAY displayMeasureFunc);
    static std::string getStringFromSenseValue(SMU_SENSE sense);
    static std::string getLetterFromUnit(UNIT unit);
    static std::string getMeasurementBufferName(SMU_CHANNEL channel);

    int m_bufferEntriesA = 1;
    int m_bufferEntriesB = 1;
    std::vector<std::string> defaultBufferedScript{CHANNEL_A_BUFFER + " = smua.makebuffer(%A_M_BUFFER_SIZE%)",
                                                   CHANNEL_B_BUFFER + " = smub.makebuffer(%B_M_BUFFER_SIZE%)",
                                                   CHANNEL_A_BUFFER + ".appendmode = 1",
                                                   CHANNEL_B_BUFFER + ".appendmode = 1"};
};
