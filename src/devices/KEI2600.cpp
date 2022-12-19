/**
 * @authors Wuhao Liu, Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include <stdexcept>
#include <iostream>
#include <regex>
#include "devices/KEI2600.h"
#include "ctlib/Logging.hpp"

extern "C" {
#include "ctlib/ErrorHandler.h"
}

/**
 * @brief Constructor initializes the ip address and timeout. Disables the logger.
 * @param ip IP-address of the KEI2600-SMU.
 * @param timeoutInMS timeout in milliseconds of the socket.
 * @param logger Logger-object to generate logging messages during the execution.
 */
KEI2600::KEI2600(const char *ip, int timeoutInMS, PIL::Logging *logger, SEND_METHOD mode) : SMU(ip, timeoutInMS, logger, mode) {
    this->m_DeviceName = DEVICE_NAME;
}

/**
 * @brief Constructor initializes the ip address and timeout. Disables the logger.
 * @param ip IP-address of the KEI2600-SMU.
 * @param timeoutInMS timeout in milliseconds of the socket.
 */
[[maybe_unused]] KEI2600::KEI2600(const char *ip, int timeoutInMS, SEND_METHOD mode) : SMU(ip, timeoutInMS, nullptr, mode) {
    this->m_DeviceName = DEVICE_NAME;
    std::string logFile = "instrument_control.log";
    m_Logger = new PIL::Logging(PIL::INFO, &logFile);
}

/**
 * @brief This function measures a certain unit on a specific channel. This function can be used to measure
 * voltage, current, power or resistance. It calls smuX.meausreUNIT().
 * @param unit Unit to measure. Allowed are voltage, current, power and resistance.
 * @param channel Channel to measure (Channel A or Channel B).
 * @param value The value which is returned by the measurement.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::measure(UNIT unit, SMU_CHANNEL channel, double *value, bool checkErrorBuffer)
{
    if (IsBuffered()) {
        throw new std::logic_error("Buffering when receiving values is currently not supported!");
    }

    auto ret = PIL_NO_ERROR;
    switch (unit)
    {
        case SMU::CURRENT:
            ret = measureI(channel, value);
            break;
        case SMU::VOLTAGE:
            ret = measureV(channel, value);
            break;
        case SMU::RESISTANCE:
            ret = measureR(channel, value);
            break;
        case SMU::POWER:
            ret = measureP(channel, value);
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Measurement method identical to KEI2600::measure, but in this method,
 * the value is directly returned to support the python wrapper.
 * @param unit Unit to measure. Allowed are voltage, current, power and resistance.
 * @param channel Channel to measure (Channel A or Channel B).
 * @return measurement value returned by the measure function.
 */
double KEI2600::measurePy(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer)
{
    if (IsBuffered()) {
        throw new std::logic_error("Buffering when receiving values is currently not supported!");
    }

    double value;
    auto ret = measure(unit, channel, &value, true);
    if (errorOccured(ret) && m_Logger)
        m_Logger->LogMessage(PIL::WARNING, __FILENAME__, __LINE__,
                             "Error while executing measure: %s", PIL_ErrorCodeToString(ret));
    if(checkErrorBuffer)
    {
        if(getErrorBufferStatus() != PIL_NO_ERROR)
            m_Logger->LogMessage(PIL::WARNING, __FILENAME__, __LINE__, getLastError());
    }
    return value;
}

/**
 * @brief This function turns on a specific channel of the KEI2600-SMU.
 * @param channel channel to enable.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::turnOn(SMU_CHANNEL channel, bool checkErrorBuffer)
{
    SubArg subArg("source",".");
           subArg.AddElem("output", ".");

    SubArg outputArg("OUTPUT_ON", ".");

    SubArg smuNumber("smu");
           smuNumber.AddElem(getChannelStringFromEnum(channel));

    ExecArgs execArgs;
    execArgs.AddArgument("smu", getChannelStringFromEnum(channel))
    .AddArgument(subArg, smuNumber, " = ")
    .AddArgument(outputArg, "");

    auto ret = Exec("", &execArgs);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief This function turns off a specific channel of the KEI2600-SMU.
 * @param channel channel to enable.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::turnOff(SMU_CHANNEL channel, bool checkErrorBuffer)
{
    SubArg subArg("source",".");
    subArg.AddElem("output", ".");

    SubArg outputArg("OUTPUT_OFF", ".");

    SubArg smuNumber("smu");
    smuNumber.AddElem(getChannelStringFromEnum(channel));

    ExecArgs execArgs;
    execArgs.AddArgument("smu", getChannelStringFromEnum(channel))
            .AddArgument(subArg, smuNumber, " = ")
            .AddArgument(outputArg, "");

    auto ret = Exec("", &execArgs);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the level of a certain channel and of a certain unit. It can set the voltage or current on Channel A
 * or Channel B.
 * @param unit unit to set allowed are voltage and current. Passing power or resistance results in a INVALID_ARGUMENTS
 * error code.
 * @param channel channel to enable.
 * @param level level to set. Encoded in amps or voltages.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setLevel(UNIT unit, SMU_CHANNEL channel, double level, bool checkErrorBuffer)
{
    SubArg subArg("");
    subArg.AddElem("source", ".");

    ExecArgs args;

    switch (unit)
    {
        case CURRENT:
            subArg.AddElem("leveli", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("levelv", ".");
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }

    args.AddArgument("smu", getChannelStringFromEnum(channel));
    args.AddArgument(subArg, level, " = ");

    auto ret = Exec("", &args);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the limit of a certain channel of a certain unit. Allowed are power voltage and current.
 * Passing resistance results in a INVALID_ARGUMENTS error code.
 * @param unit unit to set allowed are voltage and current. Passing power or resistance results in a INVALID_ARGUMENTS
 * error code.
 * @param channel channel to enable.
 * @param limit limit value in amps, voltage or watts.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setLimit(UNIT unit, SMU_CHANNEL channel, double limit, bool checkErrorBuffer)
{
    SubArg subArg("");
    subArg.AddElem("source", ".");

    ExecArgs args;

    switch (unit)
    {
        case CURRENT:
            subArg.AddElem("limiti", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("limitv", ".");
            break;
        case POWER:
            subArg.AddElem("limitp", ".");
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }

    args.AddArgument("smu", getChannelStringFromEnum(channel));
    args.AddArgument(subArg, limit, " = ");

    auto ret = Exec("", &args);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the SMU in auto-range mode for a certain unit, e.g. voltage or current.
 * @param unit allowed are current and voltage.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::enableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer)
{
    auto ret = measureAutoRangeHelperFunction(channel, unit, true);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the SMU in fixed range measurement mode for a certain unit, e.g. voltage or current.
 * @param unit allowed are current and voltage.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::disableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer)
{
    auto ret = measureAutoRangeHelperFunction(channel, unit, false);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;

}

/**
 * @brief Enables source auto-range mode. The SMU immediately changes range to the range most
 * appropriate for the value being sourced if that range is different than the present SMU range.
 * @param unit allowed are current and voltage.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::enableSourceAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer)
{
    SubArg subArg("");
    subArg.AddElem("source", ".");

    switch (unit)
    {
        case CURRENT:
            subArg.AddElem("autorangei", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("autorangev", ".");
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }

    SubArg subArgAutoRange("");
    subArgAutoRange.AddElem("AUTORANGE_ON", ".");

    SubArg smuArg("smu");
    smuArg.AddElem(getChannelStringFromEnum(channel));

    ExecArgs args;
    args.AddArgument("smu", getChannelStringFromEnum(channel))
            .AddArgument(subArg, smuArg, " = ") // TODO refactor
            .AddArgument(subArgAutoRange,"");
    auto ret = Exec("", &args);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;

}

/**
 * @brief Disables source auto-range mode.
 * @param unit allowed are current and voltage.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::disableSourceAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer)
{
    SubArg subArg("");
    subArg.AddElem("source", ".");

    switch (unit)
    {
        case CURRENT:
            subArg.AddElem("autorangei", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("autorangev", ".");
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }

    SubArg subArgAutoRange("");
    subArgAutoRange.AddElem("AUTORANGE_OFF", ".");

    SubArg smuArg("smu");
    smuArg.AddElem(getChannelStringFromEnum(channel));

    ExecArgs args;
    args.AddArgument("smu", getChannelStringFromEnum(channel))
            .AddArgument(subArg, smuArg, " = ") // TODO refactor
            .AddArgument(subArgAutoRange,"");
    auto ret = Exec("", &args);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Enables the analog filter mode for a certain channel. This engages an approximately 1 Hz analog
 * filter across the current range elements. The analog filter is only active when using the 1 nA and 100 pA
 * measurement ranges.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
[[maybe_unused]] PIL_ERROR_CODE KEI2600::enableMeasureAnalogFilter(SMU_CHANNEL channel, bool checkErrorBuffer)
{
    auto ret = analogFilterHelperFunction(channel, true);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Disables the analog filter mode of a certain channel.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::disableMeasureAnalogFilter(SMU_CHANNEL smuChannel, bool checkErrorBuffer)
{
    auto ret = analogFilterHelperFunction(smuChannel, false);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the measure range (positive full-scale value of the measurement range of that SMU).
 * Explicitly setting a measure range disables measure auto-ranging for that function.
 * @param unit allowed are current and voltage.
 * @param channel selected channel either channel A or channel B.
 * @param rangeValue range value to set.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasureRange(UNIT unit, SMU_CHANNEL channel, double rangeValue, bool checkErrorBuffer)
{
    SubArg subArg("measure", ".");
    switch (unit)
    {
        case CURRENT:
            subArg.AddElem("rangei", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("rangev", ".");
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }

    ExecArgs args;
    args.AddArgument("smu", getChannelStringFromEnum(channel))
        .AddArgument(subArg, rangeValue, " = ");

    auto ret = Exec("", &args);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Assigning a value to this attribute sets the SMU to a fixed range large enough to source the assigned value.
 * The instrument selects the best range for sourcing a value of rangeValue.
 * @param unit allowed are current and voltage.
 * @param channel selected channel either channel A or channel B.
 * @param rangeValue range value to set.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setSourceRange(UNIT unit, SMU_CHANNEL channel, double rangeValue, bool checkErrorBuffer)
{
    SubArg subArg("source", ".");
    switch (unit)
    {
        case CURRENT:
            subArg.AddElem("rangei", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("rangev", ".");
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }

    ExecArgs args;
    args.AddArgument("smu", getChannelStringFromEnum(channel))
        .AddArgument(subArg, rangeValue, " = ");

    auto ret = Exec("", &args);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the sense mode of a specific channel. LOCAL, REMOTE or CALA is supported.
 * When selecting LOCAL, source-measure operations are performed using a 2-wire local sense connection.
 * If REMOTE is selected, 4-wire remote sense connections are used. CALA is the calibration sense mode.
 * @param channel selected channel either channel A or channel B.
 * @param senseArg mode to set either LOCAL, REMOTE or CALIBRATION.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setSenseMode(SMU::SMU_CHANNEL channel, SMU::SMU_SENSE senseArg, bool checkErrorBuffer)
{
    SubArg subArg("sense", ".");
    SubArg smuArg("smu");
    smuArg.AddElem(getChannelStringFromEnum(channel));

    SubArg localSenseArg(getStringFromSenseValue(senseArg), ".");
    ExecArgs execArgs;
    execArgs.AddArgument("smu", getChannelStringFromEnum(channel))
            .AddArgument(subArg, smuArg, " = ")
            .AddArgument(localSenseArg, "");

    auto ret = Exec("", &execArgs);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the integration apeture for measurements.
 * @param channel channel which should be set
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasurePLC(SMU_CHANNEL channel, double value, bool checkErrorBuffer)
{
    if(value < 0.001 || value > 25)
        return PIL_INVALID_ARGUMENTS;

    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
    .AddElem("measure", ".")
    .AddElem("nplc", ".");

    ExecArgs args;
    args.AddArgument(subArg, value, " = ");

    auto ret = Exec("", &args);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the lowest measurement range that is used when the instrument is autoranging.
 * @param unit low range unit to set. Allowed are Current and voltage.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param value value to set.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasureLowRange(UNIT unit, SMU_CHANNEL channel, double value, bool checkErrorBuffer)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("lowrange");

    switch (unit)
    {

        case VOLTAGE:
            subArg.AddElem("v");
            break;
        case CURRENT:
            subArg.AddElem("i");
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, value, " = ");

    auto ret = Exec("", &execArgs);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;

}

/**
 * @brief Enables or disableds automatic updates to the internal reference measurements (autozero) of the instrument.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param autoZero can be set either to OFF, AUTO or ONCE. (See AUTOZERO enum)
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasureAutoZero(SMU_CHANNEL channel, AUTOZERO autoZero, bool checkErrorBuffer)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
    .AddElem("measure", ".")
    .AddElem("autozero", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu"+ getChannelStringFromEnum(channel)+"."+getStringFromAutoZeroEnum(autoZero), " = ");

    auto ret = Exec("", &arg);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the number of measurements made when a measurement is requested.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param nrOfMeasurements number of measurements made.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasureCount(SMU_CHANNEL channel, int nrOfMeasurements, bool checkErrorBuffer)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("count", ".");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, nrOfMeasurements, " = ");

    auto ret = Exec("", &execArgs);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Set the source function which can bei either voltage or current.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param srcFunc Voltage or Current function to set.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setSourceFunction(SMU_CHANNEL channel, SRC_FUNC srcFunc, bool checkErrorBuffer)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("func", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu"+ getChannelStringFromEnum(channel)+"."+getStringFromSrcFuncEnum(srcFunc), " = ");

    auto ret = Exec("", &arg);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the source output-off mode
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param offMode can be either a OUTPUT_NORMAL defined by offfunc, ZERO or HIGH_Z.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setSourceOffMode(SMU_CHANNEL channel, SRC_OFF_MODE offMode, bool checkErrorBuffer)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("offmode", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu" + getChannelStringFromEnum(channel) + "." + getStringFromOffModeEnum(offMode), " = ");

    auto ret =  Exec("", &arg);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


/**
 * @brief Set the source settling mode. (See SRC_SETTLING enum for more information)
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param srcSettling settling mode. (See SRC_SETTLING enum for more information)
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setSourceSettling(SMU::SMU_CHANNEL channel, SRC_SETTLING srcSettling, bool checkErrorBuffer)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("settling", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu" + getChannelStringFromEnum(channel) + "." + getStringFromSettleEnum(srcSettling), " = ");

    auto ret = Exec("", &arg);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Turns on the source sink mode.
 * This reduces the source limit inaccuracy that occurs when operating in quadrants II and IV
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::enableSourceSink(SMU_CHANNEL channel, bool checkErrorBuffer)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("sink", ".");

    ExecArgs arg;
    arg.AddArgument(subArg, "1", " = ");

    auto ret = Exec("", &arg);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Turns off the source sink mode.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::disableSourceSink(SMU_CHANNEL channel, bool checkErrorBuffer)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("sink", ".");

    ExecArgs arg;
    arg.AddArgument(subArg, "0", " = ");

    auto ret = Exec("", &arg);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Specify the type of measurement currently displayed.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param measureFunc show amps, volts, ohms or watts.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::displayMeasureFunction(SMU::SMU_CHANNEL channel, SMU_DISPLAY measureFunc, bool checkErrorBuffer)
{
    SubArg subArg("display");
    subArg.AddElem("smu", ".")
    .AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("func", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "display" + std::string(".") +
    getStringFromMeasureDisplayFunction(measureFunc), " = ");

    auto ret = Exec("", &arg);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Enable beeper on the SMU. This command is mandatory to play beeping sounds afterwards.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::enableBeep(bool checkErrorBuffer)
{
    auto ret = enableDisableBeepHelperFunction(true);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Disable beeper on the SMU. This command is mandatory to play beeping sounds afterwards.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::disableBeep(bool checkErrorBuffer)
{
    auto ret = enableDisableBeepHelperFunction(false);
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Send a beeping sound with a specific duration and frequency to the SMU.
 * @param timeInSeconds time in seconds to play the beeping sound.
 * @param frequency frequency in HZ of the sound to play.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::beep(float timeInSeconds, int frequency, bool checkErrorBuffer)
{
    auto ret = Exec("beeper.beep(" + std::to_string(timeInSeconds) + "," + std::to_string(frequency) + ")");
    if(errorOccured(ret))
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Helper function to measure the current of a specific channel.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param value Value which is returned from the measurement function.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::measureI(SMU_CHANNEL channel, double *value)
{
    if(!value)
        return PIL_INVALID_ARGUMENTS;

    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("i()", ".");

    ExecArgs args;
    args.AddArgument(subArg, "");

    auto ret = Exec("reading = ", &args);
    if (errorOccured(ret))
        return ret;

    char buffer[MEASURE_RET_BUFF_SIZE] = {0};

    SubArg subArgPrint("");
    subArgPrint.AddElem("reading", "(", ")");

    ExecArgs execArgs;
    execArgs.AddArgument(subArgPrint, "");

    ret = Exec("print", &execArgs, buffer);
    if (errorOccured(ret))
        return ret;

    if(m_Logger)
        m_Logger->LogMessage(PIL::DEBUG, __FILENAME__, __LINE__,"measureI returned: %s", buffer);
    *value = std::stod(buffer);
    return PIL_NO_ERROR;
}

/**
 * @brief Helper function to measure the voltage of a specific channel.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param value Value which is returned from the measurement function.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::measureV(SMU_CHANNEL channel, double *value)
{
    if(!value)
        return PIL_INVALID_ARGUMENTS;

    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("v()", ".");

    ExecArgs args;
    args.AddArgument(subArg, "");

    auto ret = Exec("reading = ", &args);
    if (errorOccured(ret))
        return ret;

    char buffer[MEASURE_RET_BUFF_SIZE] = {0};

    SubArg subArgPrint("");
    subArgPrint.AddElem("reading", "(", ")");

    ExecArgs execArgs;
    execArgs.AddArgument(subArgPrint, "");

    ret = Exec("print", &execArgs, buffer);
    if (errorOccured(ret))
        return ret;

    if(m_Logger)
        m_Logger->LogMessage(PIL::DEBUG, __FILENAME__, __LINE__,"measureV returned: %s", buffer);
    *value = std::stod(buffer);
    return PIL_NO_ERROR;
}

/**
 * @brief Helper function to measure the resistance of a specific channel.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param value Value which is returned from the measurement function.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::measureR(SMU_CHANNEL channel, double *value)
{
    if(!value)
        return PIL_INVALID_ARGUMENTS;

    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("r()", ".");

    ExecArgs args;
    args.AddArgument(subArg, "");

    auto ret = Exec("reading = ", &args);
    if (errorOccured(ret))
        return ret;

    char buffer[MEASURE_RET_BUFF_SIZE] = {0};

    SubArg subArgPrint("");
    subArgPrint.AddElem("reading", "(", ")");

    ExecArgs execArgs;
    execArgs.AddArgument(subArgPrint, "");

    ret = Exec("print", &execArgs, buffer);
    if (errorOccured(ret))
        return ret;

    if(m_Logger)
        m_Logger->LogMessage(PIL::DEBUG, __FILENAME__, __LINE__,"measureR returned: %s", buffer);
    *value = std::stod(buffer);
    return PIL_NO_ERROR;
}

/**
 * @brief Helper function to measure the power in watts of a specific channel.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param value Value which is returned from the measurement function.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::measureP(SMU_CHANNEL channel, double *value)
{
    if(!value)
        return PIL_INVALID_ARGUMENTS;

    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("p()", ".");

    ExecArgs args;
    args.AddArgument(subArg, "");

    auto ret = Exec("reading = ", &args);
    if (errorOccured(ret))
        return ret;

    char buffer[MEASURE_RET_BUFF_SIZE] = {0};

    SubArg subArgPrint("");
    subArgPrint.AddElem("reading", "(", ")");

    ExecArgs execArgs;
    execArgs.AddArgument(subArgPrint, "");

    ret = Exec("print", &execArgs, buffer);
    if (errorOccured(ret))
        return ret;

    if(m_Logger)
        m_Logger->LogMessage(PIL::DEBUG, __FILENAME__, __LINE__,"measureP returned: %s", buffer);
    *value = std::stod(buffer);
    return PIL_NO_ERROR;
}

/**
 * @brief Helper function returning the channel as string which can be used within the TCP command send to the SMU.
 * @param channel channel enum to convert to a string.
 * @return SMU channel as string.
 */
/*static*/ std::string KEI2600::getChannelStringFromEnum(SMU_CHANNEL channel)
{
    switch (channel)
    {
        case SMU::CHANNEL_A:
            return "a";
        case SMU::CHANNEL_B:
            return "b";
        default:
            return "a"; // find better solution
    }
}

/**
 * @brief Helper function returning the auto zero enum as string which can be
 * used within the TCP command send to the SMU.
 * @param autoZero autoZero enum to convert to a string
 * @return autoZero as string.
 */
/*static*/ std::string KEI2600::getStringFromAutoZeroEnum(AUTOZERO autoZero)
{
    switch (autoZero)
    {
        case OFF:
            return "AUTOZERO_OFF";
        case ONCE:
            return "AUTOZERO_ONCE";
        case AUTO:
            return "AUTOZERO_AUTO";
    }
    return "AUTOZERO_OFF";
}

/**
 * @brief Helper function returning the source function enum as string which can be
 * used within the TCP command send to the SMU.
 * @param srcFunc source function enum to convert to a string
 * @return source function as string.
 */
/*static*/ std::string KEI2600::getStringFromSrcFuncEnum(SRC_FUNC srcFunc)
{
    switch (srcFunc)
    {
        case DC_AMPS:
            return "OUTPUT_DCAMPS";
        case DC_VOLTS:
            return "OUTPUT_DCVOLTS";
    }
    return "OUTPUT_DCAMPS";
}

/**
 * @brief Helper function returning the source off mode enum as string which can be
 * used within the TCP command send to the SMU.
 * @param offMode source off mode enum to convert to a string
 * @return source off mode as string.
 */
/*static*/ std::string KEI2600::getStringFromOffModeEnum(SRC_OFF_MODE offMode)
{
    switch (offMode)
    {
        case OUTPUT_NORMAL:
            return "OUTPUT_NORMAL";
        case OUTPUT_ZERO:
            return "OUTPUT_ZERO";
        case OUTPUT_HIGH_Z:
            return "OUTPUT_HIGH_Z";
    }
    return "OUTPUT_NORMAL";
}

/**
 * @brief Helper function returning the source settling enum as string which can be
 * used within the TCP command send to the SMU.
 * @param srcSettling source settling enum to convert to a string
 * @return source settling as string.
 */
/*static*/ std::string KEI2600::getStringFromSettleEnum(SRC_SETTLING srcSettling)
{
    switch (srcSettling)
    {
        case SMOOTH:
            return "SETTLE_SMOOTH";
        case FAST_RANGE:
            return "SETTLE_FAST_RANGE";
        case FAST_POLARITY:
            return "SETTLE_FAST_POLARITY";
        case DIRECT_IRANGE:
            return "SETTLE_DIRECT_IRANGE";
        case SMOOTH_100NA:
            return "SETTLE_SMOOTH_100NA";
        case FAST_ALL:
            return "SETTLE_FAST_ALL";
    }
    return "SETTLE_SMOOTH";
}

/**
 * @brief Helper function returning the source display enum as string which can be
 * used within the TCP command send to the SMU.
 * @param displayMeasureFunc source display enum to convert to a string
 * @return source display enum as string.
 */
/*static*/ std::string KEI2600::getStringFromMeasureDisplayFunction(SMU_DISPLAY displayMeasureFunc)
{
    switch (displayMeasureFunc)
    {
        case MEASURE_DC_AMPS:
            return "MEASURE_DCAMPS";
        case MEASURE_DC_VOLTS:
            return "MEASURE_DCVOLTS";
        case MEASURE_OHMS:
            return "MEASURE_OHMS";
        case MEASURE_WATTS:
            return "MEASURE_WATTS";
    }
    return "MEASURE_DC_AMPS";
}

/**
 * @brief Helper function returning the sense enum as string which can be
 * used within the TCP command send to the SMU.
 * @param sense sense enum to convert to a string
 * @return autoZero as string.
 */
/*static*/ std::string KEI2600::getStringFromSenseValue(SMU::SMU_SENSE sense)
{
    switch (sense)
    {
        case LOCAL:
            return "SENSE_LOCAL";
        case REMOTE:
            return "SENSE_REMOTE";
        case CALIBRATION:
            return "SENSE_CALA";
    }
    return "SENSE_LOCAL";
}

/**
 * @brief Helper function to enable or disable filter helper functions.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param enable enable or disable the analog filter.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::analogFilterHelperFunction(SMU::SMU_CHANNEL channel, bool enable)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("analogfilter", ".");

    ExecArgs arg;
    if (enable)
        arg.AddArgument(subArg, "1", " = ");
    else
        arg.AddArgument(subArg, "0", " = ");

    return Exec("", &arg);
}

/**
 * @brief Helper function to set the measure auto range, for different units.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param unit Unit to measure. Allowed are voltage, current, power and resistance.
 * @param enable enable or disable the auto range filter function.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::measureAutoRangeHelperFunction(SMU_CHANNEL channel, UNIT unit, bool enable)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".");

    switch (unit)
    {
        case CURRENT:
            subArg.AddElem("autorangei", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("autorangev", ".");
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }

    ExecArgs args;
    if(enable)
        args.AddArgument(subArg, "1", " = ");
    else
        args.AddArgument(subArg, "0", " = ");

    return Exec("", &args);
}

/**
 * @brief Helper function to enable or disable beep functionality.
 * @param enable select if the beep function is enabled or disabled.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::enableDisableBeepHelperFunction(bool enable)
{
    SubArg subArg("beeper");
    subArg.AddElem("enable", ".");

    ExecArgs execArgs;
    if(enable)
        execArgs.AddArgument(subArg, "beeper.ON", " = ");
    else
        execArgs.AddArgument(subArg, "beeper.OFF", " = ");

    return Exec("", &execArgs);
}

/**
 * @brief Return last error in error-queue.
 * @return Return last error from error-queue as string.
 */
std::string KEI2600::getLastError()
{
    if (IsBuffered()) {
        throw new std::logic_error("Buffering when receiving values is currently not supported!");
    }

    ExecArgs argsErrorQueue;
    argsErrorQueue.AddArgument("errorcode, message = errorqueue.next()", "");
    auto ret = Exec("", &argsErrorQueue);
    if(errorOccured(ret))
        return "INTERNAL ERROR";

    char errorBuffer[2048];
    ExecArgs argsPrintError;
    argsPrintError.AddArgument("print(errorcode, message)", "");
    ret = Exec("", &argsPrintError, errorBuffer, true, 2048);
    if(errorOccured(ret))
        return "INTERNAL ERROR";
    std::string  retStr = std::string(errorBuffer);
    return retStr.substr(0, retStr.find('\n'));
}

/**
 * @brief Clear the error buffer.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::clearErrorBuffer()
{
    ExecArgs args;
    args.AddArgument("errorqueue", "clear()", ".");

    return Exec("", &args);
}

/**
 * @brief Request amount of elements in error queue. If queue is empty return PIL_NO_ERROR otherwise return
 * PIL_ITEM_IN_ERROR_QUEUE.
 * @return PIL_NO_ERROR if there is no item in the error queue. Otherwise return PIL_ITEM_IN_ERROR_QUEUE.
 * If the queue could not be requested successfully. Return a specific error code.
 */
PIL_ERROR_CODE KEI2600::getErrorBufferStatus()
{
    ExecArgs argsErrorQueue;
    argsErrorQueue.AddArgument("count = errorqueue.count", "");
    auto ret = Exec("", &argsErrorQueue);
    if(errorOccured(ret))
        return ret;

    char errorBuffer[2048];
    ExecArgs argsPrintError;
    argsPrintError.AddArgument("print(count)", "");
    ret = Exec("", &argsPrintError, errorBuffer, true, 2048);
    if(errorOccured(ret))
        return ret;

    std::string retStr = std::string(errorBuffer);
    retStr = retStr.substr(0, retStr.find('\n'));
    try
    {
    if(std::stoi(retStr) > 0)
        return PIL_ITEM_IN_ERROR_QUEUE;
    } catch (const std::invalid_argument & e)
    {
        return PIL_UNKNOWN_ERROR;
    }
    return PIL_NO_ERROR;
}

/**
 * @brief Replaces all substrings in the string with the given replacement string.
 * @param str String to replace substrings in.
 * @param from The substring to replace.
 * @param to The string to replace all matching substrings with.
 * @return The processed string
 */
std::string replaceAllSubstrings(std::string str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

/**
 * Do a linear voltage sweep on the SMU. Increases the voltage at the given rate until the stop voltage is arrived.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::performLinearVoltageSweep(SMU_CHANNEL channel, double startVoltage, double stopVoltage,
                                                  int increaseRate, double current, bool checkErrorBuffer) {
    std::string sweep = "start_voltage = " + std::to_string(startVoltage) + " * 1000\n"
                        "stop_voltage = " + std::to_string(stopVoltage) + " * 1000\n"
                        "rate = " + std::to_string(increaseRate) + "\n"
                        "current = " + std::to_string(current) + "\n"
                        "channel = smu" + getChannelStringFromEnum(channel) + "\n"
                        "channel.source.func = channel.OUTPUT_DCVOLTS\n"
                        "channel.source.output = channel.OUTPUT_ON\n"
                        "channel.source.limitv = (stop_voltage / 1000) + 0.1\n"
                        "channel.source.limiti = current + 0.0001\n"
                        "channel.source.leveli = current\n"
                        "for v = start_voltage, stop_voltage do\n"
                        "    channel.source.levelv = v / 1000\n"
                        "    delay(1 / rate)\n"
                        "end\n"
                        "channel.source.output = channel.OUTPUT_OFF\n";

    /* setSourceFunction(channel, DC_VOLTS, checkErrorBuffer);

    turnOn(channel, false);
    setLimit(VOLTAGE, channel, stopVoltage + 0.1, checkErrorBuffer);
    setLimit(CURRENT, channel, current + 0.001, checkErrorBuffer);
    setLevel(CURRENT, channel, current, checkErrorBuffer);

    double currentVoltage = startVoltage;
    while (currentVoltage < stopVoltage) {
        setLevel(VOLTAGE, channel, currentVoltage, checkErrorBuffer);
        delay(1.0 / increaseRate);
        currentVoltage += 0.001;
    }
    setLevel(VOLTAGE, channel, stopVoltage, checkErrorBuffer);

    turnOff(channel, checkErrorBuffer);
    return PIL_NO_ERROR; */

    return sendAndExecuteScript(sweep, "sweep", checkErrorBuffer);
}

/**
 * Sends the given script to the SMU. The scripts does not get ececuted.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::sendScript(std::string script, std::string scriptName, bool checkErrorBuffer) {
    std::string prefix = "function " + scriptName + "() ";
    std::string suffix = " end";
    std::string scriptOneLine = replaceAllSubstrings(script, "\n", " ");

    std::string processedScript = prefix + scriptOneLine + suffix;

    PIL_ERROR_CODE ret = Exec(processedScript, nullptr);

    if (errorOccured(ret) && m_Logger) {
        m_Logger->LogMessage(PIL::WARNING, __FILENAME__, __LINE__,
                             "Error while sending script: %s", PIL_ErrorCodeToString(ret));
    }

    if(checkErrorBuffer) {
        return getErrorBufferStatus();
    }

    return ret;
}

/**
 * Executes the script with the given name on the smu.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::executeScript(std::string scriptName, bool checkErrorBuffer) {
    std::string suffix = "()";

    std::string executeCommand = scriptName + suffix;
    PIL_ERROR_CODE ret = Exec(executeCommand);

    if (errorOccured(ret) && m_Logger) {
        m_Logger->LogMessage(PIL::WARNING, __FILENAME__, __LINE__,
                             "Error while executing script: %s", PIL_ErrorCodeToString(ret));
    }

    if(checkErrorBuffer) {
        return getErrorBufferStatus();
    }

    return ret;
}

/**
 * Sends and executes the given script.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::sendAndExecuteScript(std::string script, std::string scriptName, bool checkErrorBuffer) {
    PIL_ERROR_CODE ret = sendScript(script, scriptName, checkErrorBuffer);
    if (ret == PIL_ERROR_CODE::PIL_NO_ERROR) {
        ret = executeScript(scriptName, checkErrorBuffer);
    }
    return ret;
}

PIL_ERROR_CODE KEI2600::executeBufferedScript(bool checkErrorBuffer) {
    if (!m_BufferedScript.empty()) {
        m_SendMode = SEND_METHOD::DIREKT_SEND;
        auto ret = sendAndExecuteScript(m_BufferedScript, "bufferedScript", checkErrorBuffer);
        m_SendMode = SEND_METHOD::BUFFER_ENABLED;
        return ret;
    }

    return PIL_NO_ERROR;
}

std::vector<std::string> splitString(std::string toSplit, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = toSplit.find(delimiter, pos_start)) != std::string::npos) {
        token = toSplit.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back (toSplit.substr (pos_start));
    return res;
}

PIL_ERROR_CODE KEI2600::createBuffer(SMU::SMU_CHANNEL channel, std::string bufferName, int capacity, bool checkErrorBuffer) {
    return PIL_ERRNO;
}

PIL_ERROR_CODE KEI2600::clearBuffer(std::string bufferName, bool checkErrorBuffer) {
    return PIL_ERRNO;
}

PIL_ERROR_CODE KEI2600::readBuffer(std::string bufferName, double *buffer, bool checkErrorBuffer) {
    return PIL_ERRNO;
}

PIL_ERROR_CODE KEI2600::getBufferSize(std::string bufferName, int *value, bool checkErrorBuffer) {
    return PIL_ERRNO;
}
