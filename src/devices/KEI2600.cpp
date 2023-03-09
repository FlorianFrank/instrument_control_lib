/**
 * @authors Wuhao Liu, Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include "devices/KEI2600.h"
#include "ctlib/Logging.hpp"
#include "ctlib/Exception.h"
#include "HTTPRequest.h"

#include <utility> // std::move
#include <stdexcept> // std::invalid_argument
#include <thread>

extern "C" {
#include "ctlib/ErrorHandler.h"
}

/**
 * @brief Constructor initializes the ip address and timeout. Disables the logger.
 * @param ip IP-address of the KEI2600-SMU.
 * @param timeoutInMS timeout in milliseconds of the socket.
 * @param logger Logger-object to generate logging messages during the execution.
 */
KEI2600::KEI2600(std::string ipAddress, int timeoutInMS, PIL::Logging *logger, SEND_METHOD mode)
        : SMU(std::move(ipAddress), timeoutInMS, logger, mode) {
    m_BufferedScript = defaultBufferedScript;
}

/**
 * @brief Constructor initializes the ip address and timeout. Disables the logger.
 * @param ip IP-address of the KEI2600-SMU.
 * @param timeoutInMS timeout in milliseconds of the socket.
 */
[[maybe_unused]] KEI2600::KEI2600(std::string ipAddress, int timeoutInMS, SEND_METHOD mode)
        : SMU(std::move(ipAddress), timeoutInMS, nullptr, mode) {
    m_BufferedScript = defaultBufferedScript;
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
PIL_ERROR_CODE KEI2600::measure(UNIT unit, SMU_CHANNEL channel, double *value, bool checkErrorBuffer) {
    std::string unitLetter = getLetterFromUnit(unit);

    if (unitLetter.empty() || (!isBuffered() && !value)) {
        if (m_EnableExceptions)
            throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
        return PIL_INVALID_ARGUMENTS;
    }

    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem(unitLetter + "(" + getMeasurementStorage(channel) + ")", ".");

    ExecArgs args;
    args.AddArgument(subArg, "");

    auto ret = Exec("reading = ", &args);
    if (errorOccured(ret))
        return ret;

    if (!isBuffered()) {
        SubArg subArgPrint("");
        subArgPrint.AddElem("reading", "(", ")");

        ExecArgs execArgs;
        execArgs.AddArgument(subArgPrint, "");

        std::string result;
        ret = Exec("print", &execArgs, &result, true);
        if (errorOccured(ret))
            return ret;

        if (m_Logger)
            m_Logger->LogMessage(PIL::DEBUG, __FILENAME__, __LINE__, "measure" + unitLetter + "V returned: %s",
                                 result.c_str());
        *value = std::stod(result);
    }

    return handleErrorCode(ret, checkErrorBuffer);
}

/**
 * @brief Measurement method identical to KEI2600::measure, but in this method,
 * the value is directly returned to support the python wrapper.
 * @param unit Unit to measure. Allowed are voltage, current, power and resistance.
 * @param channel Channel to measure (Channel A or Channel B).
 * @return measurement value returned by the measure function.
 */
double KEI2600::measurePy(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer) {
    double value;
    auto ret = measure(unit, channel, &value, checkErrorBuffer);

    if (isBuffered()) {
        return 0;
    }

    if (errorOccured(ret) && m_Logger) {
        if (m_EnableExceptions)
            throw PIL::Exception(ret, __FILENAME__, __LINE__, "");
        m_Logger->LogMessage(PIL::WARNING, __FILENAME__, __LINE__,
                             "Error while executing measure: %s", PIL_ErrorCodeToString(ret));
    }
    if (checkErrorBuffer) {
        auto returnValue = getErrorBufferStatus();
        if (errorOccured(returnValue)) {
            if (m_EnableExceptions)
                throw PIL::Exception(returnValue, __FILENAME__, __LINE__, getLastError());
            m_Logger->LogMessage(PIL::WARNING, __FILENAME__, __LINE__, getLastError());
        }
    }
    return value;
}

/**
 * @brief This function turns on a specific channel of the KEI2600-SMU.
 * @param channel channel to enable.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::turnOn(SMU_CHANNEL channel, bool checkErrorBuffer) {
    return handleErrorCode(toggleChannel(channel, true), checkErrorBuffer);
}

/**
 * @brief This function turns off a specific channel of the KEI2600-SMU.
 * @param channel channel to enable.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::turnOff(SMU_CHANNEL channel, bool checkErrorBuffer) {
    return handleErrorCode(toggleChannel(channel, false), checkErrorBuffer);
}

PIL_ERROR_CODE KEI2600::toggleChannel(SMU_CHANNEL channel, bool enable) {
    SubArg subArg("source", ".");
    subArg.AddElem("output", ".");

    std::string argString = "OUTPUT_";
    argString += (enable ? "ON" : "OFF");
    SubArg outputArg(argString, ".");

    SubArg smuNumber("smu");
    smuNumber.AddElem(getChannelStringFromEnum(channel));

    ExecArgs execArgs;
    execArgs.AddArgument("smu", getChannelStringFromEnum(channel))
            .AddArgument(subArg, smuNumber, " = ")
            .AddArgument(outputArg, "");

    return Exec("", &execArgs);
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
PIL_ERROR_CODE KEI2600::setLevel(UNIT unit, SMU_CHANNEL channel, double level, bool checkErrorBuffer) {
    SubArg subArg("");
    subArg.AddElem("source", ".");

    ExecArgs args;

    switch (unit) {
        case CURRENT:
            subArg.AddElem("leveli", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("levelv", ".");
            break;
        default:
            if (m_EnableExceptions)
                throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
            return PIL_INVALID_ARGUMENTS;
    }

    args.AddArgument("smu", getChannelStringFromEnum(channel));
    args.AddArgument(subArg, level, " = ");

    return handleErrorCode(Exec("", &args), checkErrorBuffer);
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
PIL_ERROR_CODE KEI2600::setLimit(UNIT unit, SMU_CHANNEL channel, double limit, bool checkErrorBuffer) {
    SubArg subArg("");
    subArg.AddElem("source", ".");

    ExecArgs args;

    switch (unit) {
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
            if (m_EnableExceptions)
                throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
            return PIL_INVALID_ARGUMENTS;
    }

    args.AddArgument("smu", getChannelStringFromEnum(channel));
    args.AddArgument(subArg, limit, " = ");

    return handleErrorCode(Exec("", &args), checkErrorBuffer);
}

/**
 * @brief Sets the SMU in auto-range mode for a certain unit, e.g. voltage or current.
 * @param unit allowed are current and voltage.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::enableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer) {
    return handleErrorCode(toggleMeasureAutoRange(channel, unit, true), checkErrorBuffer);
}

/**
 * @brief Sets the SMU in fixed range measurement mode for a certain unit, e.g. voltage or current.
 * @param unit allowed are current and voltage.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::disableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer) {
    return handleErrorCode(toggleMeasureAutoRange(channel, unit, false), checkErrorBuffer);
}

/**
 * @brief Helper function to set the measure auto range, for different units.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param unit Unit to measure. Allowed are voltage, current, power and resistance.
 * @param enable enable or disable the auto range filter function.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::toggleMeasureAutoRange(SMU_CHANNEL channel, UNIT unit, bool enable) {
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".");

    switch (unit) {
        case CURRENT:
            subArg.AddElem("autorangei", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("autorangev", ".");
            break;
        default:
            if (m_EnableExceptions)
                throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
            return PIL_INVALID_ARGUMENTS;
    }

    ExecArgs args;
    if (enable)
        args.AddArgument(subArg, "1", " = ");
    else
        args.AddArgument(subArg, "0", " = ");

    return Exec("", &args);
}

/**
 * @brief Enables source auto-range mode. The SMU immediately changes range to the range most
 * appropriate for the value being sourced if that range is different than the present SMU range.
 * @param unit allowed are current and voltage.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::enableSourceAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer) {
    return handleErrorCode(toggleSourceAutoRange(unit, channel, true), checkErrorBuffer);
}

/**
 * @brief Disables source auto-range mode.
 * @param unit allowed are current and voltage.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::disableSourceAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer) {
    return handleErrorCode(toggleSourceAutoRange(unit, channel, false), checkErrorBuffer);
}

PIL_ERROR_CODE KEI2600::toggleSourceAutoRange(UNIT unit, SMU_CHANNEL channel, bool enable) {
    SubArg subArg("");
    subArg.AddElem("source", ".");

    switch (unit) {
        case CURRENT:
            subArg.AddElem("autorangei", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("autorangev", ".");
            break;
        default:
            if (m_EnableExceptions)
                throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
            return PIL_INVALID_ARGUMENTS;
    }

    SubArg subArgAutoRange("");

    if (enable)
        subArgAutoRange.AddElem("AUTORANGE_ON", ".");
    else
        subArgAutoRange.AddElem("AUTORANGE_OFF", ".");

    SubArg smuArg("smu");
    smuArg.AddElem(getChannelStringFromEnum(channel));

    ExecArgs args;
    args.AddArgument("smu", getChannelStringFromEnum(channel))
            .AddArgument(subArg, smuArg, " = ")
            .AddArgument(subArgAutoRange, "");

    return Exec("", &args);
}

/**
 * @brief Enables the analog filter mode for a certain channel. This engages an approximately 1 Hz analog
 * filter across the current range elements. The analog filter is only active when using the 1 nA and 100 pA
 * measurement ranges.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::enableMeasureAnalogFilter(SMU_CHANNEL channel, bool checkErrorBuffer) {
    return handleErrorCode(toggleMeasureAnalogFilter(channel, true), checkErrorBuffer);
}

/**
 * @brief Disables the analog filter mode of a certain channel.
 * @param channel selected channel either channel A or channel B.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::disableMeasureAnalogFilter(SMU_CHANNEL smuChannel, bool checkErrorBuffer) {
    return handleErrorCode(toggleMeasureAnalogFilter(smuChannel, false), checkErrorBuffer);
}

/**
 * @brief Helper function to enable or disable filter helper functions.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param enable enable or disable the analog filter.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::toggleMeasureAnalogFilter(SMU::SMU_CHANNEL channel, bool enable) {
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
 * @brief Turns on the source sink mode.
 * This reduces the source limit inaccuracy that occurs when operating in quadrants II and IV
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::enableSourceSink(SMU_CHANNEL channel, bool checkErrorBuffer) {  // TODO: refactor
    return handleErrorCode(toggleSourceSink(channel, true), checkErrorBuffer);
}

/**
 * @brief Turns off the source sink mode.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::disableSourceSink(SMU_CHANNEL channel, bool checkErrorBuffer) {
    return handleErrorCode(toggleSourceSink(channel, false), checkErrorBuffer);
}

PIL_ERROR_CODE KEI2600::toggleSourceSink(SMU_CHANNEL channel, bool enable) {
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("sink", ".");

    ExecArgs arg;
    if (enable)
        arg.AddArgument(subArg, "1", " = ");
    else
        arg.AddArgument(subArg, "0", " = ");

    return Exec("", &arg);
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
PIL_ERROR_CODE KEI2600::setMeasureRange(UNIT unit, SMU_CHANNEL channel, double rangeValue, bool checkErrorBuffer) {
    SubArg subArg("measure", ".");
    switch (unit) {
        case CURRENT:
            subArg.AddElem("rangei", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("rangev", ".");
            break;
        default:
            if (m_EnableExceptions)
                throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
            return PIL_INVALID_ARGUMENTS;
    }

    ExecArgs args;
    args.AddArgument("smu", getChannelStringFromEnum(channel))
            .AddArgument(subArg, rangeValue, " = ");

    return handleErrorCode(Exec("", &args), checkErrorBuffer);
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
PIL_ERROR_CODE KEI2600::setSourceRange(UNIT unit, SMU_CHANNEL channel, double rangeValue, bool checkErrorBuffer) {
    SubArg subArg("source", ".");
    switch (unit) {
        case CURRENT:
            subArg.AddElem("rangei", ".");
            break;
        case VOLTAGE:
            subArg.AddElem("rangev", ".");
            break;
        default:
            if (m_EnableExceptions)
                throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
            return PIL_INVALID_ARGUMENTS;
    }

    ExecArgs args;
    args.AddArgument("smu", getChannelStringFromEnum(channel))
            .AddArgument(subArg, rangeValue, " = ");

    return handleErrorCode(Exec("", &args), checkErrorBuffer);
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
PIL_ERROR_CODE KEI2600::setSenseMode(SMU::SMU_CHANNEL channel, SMU::SMU_SENSE senseArg, bool checkErrorBuffer) {
    SubArg subArg("sense", ".");
    SubArg smuArg("smu");
    smuArg.AddElem(getChannelStringFromEnum(channel));

    SubArg localSenseArg(getStringFromSenseValue(senseArg), ".");
    ExecArgs execArgs;
    execArgs.AddArgument("smu", getChannelStringFromEnum(channel))
            .AddArgument(subArg, smuArg, " = ")
            .AddArgument(localSenseArg, "");

    return handleErrorCode(Exec("", &execArgs), checkErrorBuffer);
}

/**
 * @brief Sets the integration apeture for measurements.
 * @param channel channel which should be set
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasurePLC(SMU_CHANNEL channel, double value, bool checkErrorBuffer) {
    if (value < 0.001 || value > 25) {
        if (m_EnableExceptions)
            throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
        return PIL_INVALID_ARGUMENTS;
    }

    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("nplc", ".");

    ExecArgs args;
    args.AddArgument(subArg, value, " = ");

    return handleErrorCode(Exec("", &args), checkErrorBuffer);
}

/**
 * @brief Sets the lowest measurement range that is used when the instrument is autoranging.
 * @param unit low range unit to set. Allowed are Current and voltage.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param value value to set.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasureLowRange(UNIT unit, SMU_CHANNEL channel, double value, bool checkErrorBuffer) {
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("lowrange");

    switch (unit) {

        case VOLTAGE:
            subArg.AddElem("v");
            break;
        case CURRENT:
            subArg.AddElem("i");
            break;
        default:
            if (m_EnableExceptions)
                throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
            return PIL_INVALID_ARGUMENTS;
    }

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, value, " = ");

    return handleErrorCode(Exec("", &execArgs), checkErrorBuffer);

}

/**
 * @brief Enables or disableds automatic updates to the internal reference measurements (autozero) of the instrument.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param autoZero can be set either to OFF, AUTO or ONCE. (See AUTOZERO enum)
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasureAutoZero(SMU_CHANNEL channel, AUTOZERO autoZero, bool checkErrorBuffer) {
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("autozero", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu" + getChannelStringFromEnum(channel) + "." + getStringFromAutoZeroEnum(autoZero),
                    " = ");

    return handleErrorCode(Exec("", &arg), checkErrorBuffer);
}

/**
 * @brief Sets the number of measurements made when a measurement is requested.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param nrOfMeasurements number of measurements made.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasureCount(SMU_CHANNEL channel, int nrOfMeasurements, bool checkErrorBuffer) {
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("count", ".");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, nrOfMeasurements, " = ");

    return handleErrorCode(Exec("", &execArgs), checkErrorBuffer);
}

/**
 * @brief Set the source function which can bei either voltage or current.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param srcFunc Voltage or Current function to set.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setSourceFunction(SMU_CHANNEL channel, SRC_FUNC srcFunc, bool checkErrorBuffer) {
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("func", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu" + getChannelStringFromEnum(channel) + "." + getStringFromSrcFuncEnum(srcFunc), " = ");

    return handleErrorCode(Exec("", &arg), checkErrorBuffer);
}

/**
 * @brief Sets the source output-off mode
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param offMode can be either a OUTPUT_NORMAL defined by offfunc, ZERO or HIGH_Z.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setSourceOffMode(SMU_CHANNEL channel, SRC_OFF_MODE offMode, bool checkErrorBuffer) {
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("offmode", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu" + getChannelStringFromEnum(channel) + "." + getStringFromOffModeEnum(offMode), " = ");

    return handleErrorCode(Exec("", &arg), checkErrorBuffer);
}


/**
 * @brief Set the source settling mode. (See SRC_SETTLING enum for more information)
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param srcSettling settling mode. (See SRC_SETTLING enum for more information)
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::setSourceSettling(SMU::SMU_CHANNEL channel, SRC_SETTLING srcSettling, bool checkErrorBuffer) {
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("settling", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu" + getChannelStringFromEnum(channel) + "." + getStringFromSettleEnum(srcSettling),
                    " = ");

    return handleErrorCode(Exec("", &arg), checkErrorBuffer);
}

/**
 * @brief Specify the type of measurement currently displayed.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param measureFunc show amps, volts, ohms or watts.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE
KEI2600::displayMeasureFunction(SMU::SMU_CHANNEL channel, SMU_DISPLAY measureFunc, bool checkErrorBuffer) {
    SubArg subArg("display");
    subArg.AddElem("smu", ".")
            .AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("func", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "display" + std::string(".") +
                            getStringFromMeasureDisplayFunction(measureFunc), " = ");

    return handleErrorCode(Exec("", &arg), checkErrorBuffer);
}

/**
 * @brief Enable beeper on the SMU. This command is mandatory to play beeping sounds afterwards.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::enableBeep(bool checkErrorBuffer) {
    return handleErrorCode(toggleBeep(true), checkErrorBuffer);
}

/**
 * @brief Disable beeper on the SMU. This command is mandatory to play beeping sounds afterwards.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::disableBeep(bool checkErrorBuffer) {
    return handleErrorCode(toggleBeep(false), checkErrorBuffer);
}

/**
 * @brief Helper function to enable or disable beep functionality.
 * @param enable select if the beep function is enabled or disabled.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::toggleBeep(bool enable) {
    SubArg subArg("beeper");
    subArg.AddElem("enable", ".");

    ExecArgs execArgs;
    if (enable)
        execArgs.AddArgument(subArg, "beeper.ON", " = ");
    else
        execArgs.AddArgument(subArg, "beeper.OFF", " = ");

    return Exec("", &execArgs);
}

/**
 * @brief Send a beeping sound with a specific duration and frequency to the SMU.
 * @param timeInSeconds time in seconds to play the beeping sound.
 * @param frequency frequency in HZ of the sound to play.
 * @param checkErrorBuffer if true check the error buffer after execution.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::beep(float timeInSeconds, int frequency, bool checkErrorBuffer) {
    return handleErrorCode(Exec("beeper.beep(" + std::to_string(timeInSeconds) + "," + std::to_string(frequency) + ")"),
                           checkErrorBuffer);
}

/**
 * @brief Return last error in error-queue.
 * @return Return last error from error-queue as string.
 */
std::string KEI2600::getLastError() {
    if (isBuffered()) {
        return "Currently Buffering, currently only accumulating buffered script.";
    } else {
        ExecArgs argsErrorQueue;
        argsErrorQueue.AddArgument("errorcode, message = errorqueue.next()", "");
        auto ret = Exec("", &argsErrorQueue);
        if (errorOccured(ret))
            return "INTERNAL ERROR";

        std::string errorBuffer;
        ExecArgs argsPrintError;
        argsPrintError.AddArgument("print(errorcode, message)", "");
        ret = Exec("", &argsPrintError, &errorBuffer, true);
        if (errorOccured(ret))
            return "INTERNAL ERROR";
        return errorBuffer.substr(0, errorBuffer.find('\n'));
    }
}

/**
 * @brief Clear the error buffer.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::clearErrorBuffer() {
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
PIL_ERROR_CODE KEI2600::getErrorBufferStatus() {
    ExecArgs argsErrorQueue;
    argsErrorQueue.AddArgument("count = errorqueue.count", "");
    auto ret = Exec("", &argsErrorQueue);
    if (errorOccured(ret))
        return ret;

    std::string errorBufferRet;
    ExecArgs argsPrintError;
    argsPrintError.AddArgument("print(count)", "");
    ret = Exec("", &argsPrintError, &errorBufferRet, true);
    if (errorOccured(ret))
        return ret;

    errorBufferRet = errorBufferRet.substr(0, errorBufferRet.find('\n'));
    try {
        if (std::stoi(errorBufferRet) > 0) {
            if (m_EnableExceptions)
                throw PIL::Exception(PIL_ITEM_IN_ERROR_QUEUE, __FILENAME__, __LINE__, "");
            return PIL_ITEM_IN_ERROR_QUEUE;
        }
    } catch (const std::invalid_argument &e) {
        throw PIL::Exception(PIL_INVALID_ARGUMENTS, e.what());
    }
    return PIL_NO_ERROR;
}

/**
 * @brief Perform a linear voltage sweep on the SMU. Increases the voltage at the given rate until the stop voltage is arrived.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::performLinearVoltageSweep(SMU_CHANNEL channel, double startVoltage, double stopVoltage,
                                                  int increaseRate_mVpS, double current, bool checkErrorBuffer) {
    /* A smaller script to directly send to the smu. If we buffer the loop gets unrolled which can get very long.
    std::string sweep = "start_voltage = " + std::to_string(startVoltage) + " * 1000\n"
                        "stop_voltage = " + std::to_string(stopVoltage) + " * 1000\n"
                        "rate = " + std::to_string(increaseRate_mVpS)) + "\n"
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
    return sendAndExecuteVectorScript(sweep, "sweep", checkErrorBuffer);
    */

    SEND_METHOD prevSendMethod = m_SendMode;
    std::vector<std::string> oldBuffer = m_BufferedScript;
    m_BufferedScript.clear();
    changeSendMode(BUFFER_ENABLED);

    setSourceFunction(channel, DC_VOLTS, checkErrorBuffer);
    turnOn(channel, checkErrorBuffer);
    setLimit(VOLTAGE, channel, stopVoltage + 0.1, checkErrorBuffer);
    setLimit(CURRENT, channel, current + 0.001, checkErrorBuffer);
    setLevel(CURRENT, channel, current, checkErrorBuffer);
    double currentVoltage = startVoltage;
    while (currentVoltage < stopVoltage) {
        setLevel(VOLTAGE, channel, currentVoltage, checkErrorBuffer);
        delay(1.0 / increaseRate_mVpS);
        currentVoltage += 0.001;
    }
    setLevel(VOLTAGE, channel, stopVoltage, checkErrorBuffer);
    turnOff(channel, checkErrorBuffer);

    auto ret = executeBufferedScript(checkErrorBuffer);

    m_SendMode = prevSendMethod;
    m_BufferedScript = oldBuffer;

    return handleErrorCode(ret, checkErrorBuffer);
}

/**
 * @brief Creates a payload of a post request for the smu. 
 * 
 * @param value The value of the payload.
 * @return The constructed payload.
 */
std::string KEI2600::createPayload(const std::string &value) {
    return R"({"command": "shellInput", "value": ")" + value + "\"}";
}

/**
 * @brief Create a payload provided a vector of values.
 * Works by splitting the values into batches and constructing a payload.
 * 
 * @param offset The value to start collecting values.
 * @param numberOfLines The total number of lines the batch is going to have at max.
 * @param values The values of the payload.
 * @param result The vector to insert the payload to.
 */
void KEI2600::createPayloadBatch(int offset, int numberOfLines, std::vector<std::string> values,
                                 std::vector<std::string> *result) {
    std::string value;
    for (int i = offset; i < offset + numberOfLines; ++i) {
        value += values[i] + "\n";
    }

    result->push_back(createPayload(value));
}

/**
 * @brief Sends the given script to the SMU. The scripts does not get executed.
 * 
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::sendScript(const std::string &scriptName, const std::string &script, bool checkErrorBuffer) {
    return sendVectorScript(scriptName, splitString(script, "\n"), checkErrorBuffer);
}

/**
 * @brief Sends the given script to the SMU. The scripts does not get executed.
 *
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::sendVectorScript(const std::string &scriptName, const std::vector<std::string> &script,
                                         bool checkErrorBuffer) {
    std::string url = "http://" + m_IPAddr + "/HttpCommand";

    std::vector<std::string> sendableScript = script;
    sendableScript.insert(sendableScript.begin(), "loadscript " + scriptName);
    sendableScript.emplace_back("endscript");

    std::string exitPayload = R"({"command": "keyInput", "value": "K"})";

    int batchSize = 32;
    int numberOfLines = sendableScript.size(); // NOLINT(cppcoreguidelines-narrowing-conversions)
    int numberOfBatches = numberOfLines / batchSize;
    int remaining = numberOfLines % batchSize;

    std::vector<std::string> payloads;

    for (int i = 0; i < numberOfBatches; ++i) {
        int offset = i * batchSize;
        createPayloadBatch(offset, batchSize, sendableScript, &payloads);
    }

    if (remaining > 0) {
        int offset = numberOfBatches * batchSize;
        createPayloadBatch(offset, remaining, sendableScript, &payloads);
    }

    payloads.push_back(createPayload(scriptName + ".save()"));

    auto ret = postRequest(url, exitPayload);
    if (errorOccured(ret)) {
        return handleErrorCode(ret, checkErrorBuffer);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    for (auto &payload: payloads) {
        ret = postRequest(url, payload);
        if (errorOccured(ret)) {
            return handleErrorCode(ret, checkErrorBuffer);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    ret = postRequest(url, exitPayload);
    return handleErrorCode(ret, checkErrorBuffer);
}

/**
 * @brief Executes the script with the given name on the smu.
 * 
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::executeScript(const std::string &scriptName, bool checkErrorBuffer) {
    std::string suffix = "()";

    std::string executeCommand = scriptName + suffix;
    auto ret = Exec(executeCommand);

    if (errorOccured(ret) && m_Logger) {
        m_Logger->LogMessage(PIL::WARNING, __FILENAME__, __LINE__,
                             "Error while executing script: %s", PIL_ErrorCodeToString(ret));
    }

    return handleErrorCode(ret, checkErrorBuffer);
}

/**
 * @brief Sends and executes the given script.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::sendAndExecuteScript(const std::string &scriptName, const std::string &script,
                                             bool checkErrorBuffer) {
    return sendAndExecuteVectorScript(scriptName, splitString(script, "\n"), checkErrorBuffer);
}

/**
 * @brief Sends and executes the given script.
 * @param checkErrorBuffer if true error buffer status is requested and evaluated.
 * @return NO_ERROR if execution was successful otherwise return error code.
 */
PIL_ERROR_CODE KEI2600::sendAndExecuteVectorScript(const std::string &scriptName,
                                                   const std::vector<std::string> &script, bool checkErrorBuffer) {
    PIL_ERROR_CODE ret = sendVectorScript(scriptName, script, checkErrorBuffer);

    if (!errorOccured(ret)) {
        ret = executeScript(scriptName, checkErrorBuffer);
    }

    return handleErrorCode(ret, checkErrorBuffer);
}

/**
 * @brief Executes the buffered script.
 * 
 * @param checkErrorBuffer Whether to check the error buffer after executing.
 * @return The received error code.
 */
PIL_ERROR_CODE KEI2600::executeBufferedScript(bool checkErrorBuffer) {
    SEND_METHOD prevSendMode = m_SendMode;
    m_SendMode = SEND_METHOD::DIRECT_SEND;

    m_BufferedScript[0] = replaceAllSubstrings(m_BufferedScript[0], "%A_M_BUFFER_SIZE%",
                                               std::to_string(m_bufferEntriesA));
    m_BufferedScript[1] = replaceAllSubstrings(m_BufferedScript[1], "%B_M_BUFFER_SIZE%",
                                               "" + std::to_string(m_bufferEntriesB));

    std::string s = vectorToStringNL(m_BufferedScript);
    auto ret = sendAndExecuteVectorScript("bufferedScript", m_BufferedScript, checkErrorBuffer);
    m_SendMode = prevSendMode;
    clearBufferedScript();

    return handleErrorCode(ret, checkErrorBuffer);
}

/**
 * @brief Clears the buffer with the given name.
 * 
 * @param bufferName The name of the buffer.
 * @param checkErrorBuffer Whether to check the error buffer after executing.
 * @return The received error code.
 */
PIL_ERROR_CODE KEI2600::clearBuffer(const std::string &bufferName, bool checkErrorBuffer) {
    SubArg subArg(bufferName);
    subArg.AddElem("clear", ".")
            .AddElem("", "(", ")");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, "");

    auto ret = Exec("", &execArgs, nullptr);
    return handleErrorCode(ret, checkErrorBuffer);
}

/**
 * @brief Reads part of the buffer with the given name.
 * 
 * @return The received error code.
 */
PIL_ERROR_CODE KEI2600::readPartOfBuffer(int startIdx, int endIdx, const std::string &bufferName, char *printBuffer,
                                         std::vector<double> *result, bool checkErrorBuffer) {
    SubArg subArg("");
    subArg.AddElem(std::to_string(startIdx) + ", " + std::to_string(endIdx) + ", " + bufferName, "(", ")");
    ExecArgs execArgs;
    execArgs.AddArgument(subArg, "");
    auto ret = Exec("printbuffer", &execArgs, printBuffer);

    if (errorOccured(ret)) {
        return handleErrorCode(ret, checkErrorBuffer);
    }

    for (const std::string &value: splitString(std::string(printBuffer), ", ")) {
        result->push_back(std::stod(value));
    }

    return handleErrorCode(ret, checkErrorBuffer);
}

/**
 * @brief Reads the complete buffer with the given name.
 * 
 * @return The received error code.
 */
PIL_ERROR_CODE KEI2600::readBuffer(const std::string &bufferName, std::vector<double> *result, bool checkErrorBuffer) {
    int n = 0;
    getBufferSize(bufferName, &n, checkErrorBuffer);

    int doubleSize = 15;
    int batchSize = 1024 / doubleSize;
    int batches = n / batchSize;
    int remaining = n % batchSize;

    char printBuffer[1024];

    result->reserve(n);
    for (int i = 0; i < batches; ++i) {
        int offset = i * batchSize;
        appendToBuffer(1 + offset, offset + batchSize, bufferName, printBuffer, result, checkErrorBuffer);
    }

    if (remaining > 0) {
        int offset = batches * batchSize;
        appendToBuffer(1 + offset, offset + remaining, bufferName, printBuffer, result, checkErrorBuffer);
    }

    return clearBuffer(bufferName, checkErrorBuffer);
}

PIL_ERROR_CODE KEI2600::appendToBuffer(int startIdx, int endIdx, const std::string &bufferName, char *printBuffer,
                                       std::vector<double> *result, bool checkErrorBuffer) {
    std::vector<double> batchVector;
    auto ret = readPartOfBuffer(startIdx, endIdx, bufferName, printBuffer, &batchVector, checkErrorBuffer);
    if (errorOccured(ret)) {
        return handleErrorCode(ret, checkErrorBuffer);
    }

    for (double value: batchVector) {
        result->push_back(value);
    }

    return PIL_NO_ERROR;
}

std::vector<double> KEI2600::readBufferPy(const std::string &bufferName, bool checkErrorBuffer) {
    std::vector<double> buffer;
    readBuffer(bufferName, &buffer, checkErrorBuffer);
    return buffer;
}

/**
 * @brief Retrieves the size of the buffer with the given name.
 * 
 * @return The received error code.
 */
PIL_ERROR_CODE KEI2600::getBufferSize(const std::string &bufferName, int *value, bool checkErrorBuffer) {
    char sizeBuffer[16];

    SubArg subArg("");
    subArg.AddElem(bufferName + ".n", "(", ")");
    ExecArgs execArgs;
    execArgs.AddArgument(subArg, "");

    auto ret = Exec("print", &execArgs, sizeBuffer);

    int n = (int) std::stod(sizeBuffer);
    *value = n;

    return handleErrorCode(ret, checkErrorBuffer);
}

void KEI2600::clearBufferedScript() {
    m_BufferedScript = defaultBufferedScript;
    m_bufferEntriesA = 1;
    m_bufferEntriesB = 1;
}

/**
 * @brief Checks the given error code and checks the error buffer if specified.
 *
 * @param errorCode The error code to check.
 * @param checkErrorBuffer Whether to check the error buffer.
 * @return The corresponding error code.
 */
PIL_ERROR_CODE KEI2600::handleErrorCode(PIL_ERROR_CODE errorCode, bool checkErrorBuffer) {
    if (errorOccured(errorCode))
        return errorCode;
    if (!isBuffered() && checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Helper function returning the channel as string which can be used within the TCP command send to the SMU.
 * @param channel channel enum to convert to a string.
 * @return SMU channel as string. Return empty string if channel is invalid.
 */
std::string KEI2600::getChannelStringFromEnum(SMU_CHANNEL channel) {
    switch (channel) {
        case SMU::CHANNEL_A:
            return "a";
        case SMU::CHANNEL_B:
            return "b";
        default:
            throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
    }
}

/**
 * @brief Helper function returning the auto zero enum as string which can be
 * used within the TCP command send to the SMU.
 * @param autoZero autoZero enum to convert to a string
 * @return autoZero as string.
 */
/*static*/ std::string KEI2600::getStringFromAutoZeroEnum(AUTOZERO autoZero) {
    switch (autoZero) {
        case OFF:
            return "AUTOZERO_OFF";
        case ONCE:
            return "AUTOZERO_ONCE";
        case AUTO:
            return "AUTOZERO_AUTO";
        default:
            return "AUTOZERO_OFF";
    }
}

/**
 * @brief Helper function returning the source function enum as string which can be
 * used within the TCP command send to the SMU.
 * @param srcFunc source function enum to convert to a string
 * @return source function as string.
 */
/*static*/ std::string KEI2600::getStringFromSrcFuncEnum(SRC_FUNC srcFunc) {
    switch (srcFunc) {
        case DC_AMPS:
            return "OUTPUT_DCAMPS";
        case DC_VOLTS:
            return "OUTPUT_DCVOLTS";
        default:
            return "OUTPUT_DCAMPS";
    }
}

/**
 * @brief Helper function returning the source off mode enum as string which can be
 * used within the TCP command send to the SMU.
 * @param offMode source off mode enum to convert to a string
 * @return source off mode as string.
 */
/*static*/ std::string KEI2600::getStringFromOffModeEnum(SRC_OFF_MODE offMode) {
    switch (offMode) {
        case OUTPUT_NORMAL:
            return "OUTPUT_NORMAL";
        case OUTPUT_ZERO:
            return "OUTPUT_ZERO";
        case OUTPUT_HIGH_Z:
            return "OUTPUT_HIGH_Z";
        default:
            return "OUTPUT_NORMAL";
    }
}

/**
 * @brief Helper function returning the source settling enum as string which can be
 * used within the TCP command send to the SMU.
 * @param srcSettling source settling enum to convert to a string
 * @return source settling as string.
 */
/*static*/ std::string KEI2600::getStringFromSettleEnum(SRC_SETTLING srcSettling) {
    switch (srcSettling) {
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
        default:
            return "SETTLE_SMOOTH";
    }
}

/**
 * @brief Helper function returning the source display enum as string which can be
 * used within the TCP command send to the SMU.
 * @param displayMeasureFunc source display enum to convert to a string
 * @return source display enum as string.
 */
/*static*/ std::string KEI2600::getStringFromMeasureDisplayFunction(SMU_DISPLAY displayMeasureFunc) {
    switch (displayMeasureFunc) {
        case MEASURE_DC_AMPS:
            return "MEASURE_DCAMPS";
        case MEASURE_DC_VOLTS:
            return "MEASURE_DCVOLTS";
        case MEASURE_OHMS:
            return "MEASURE_OHMS";
        case MEASURE_WATTS:
            return "MEASURE_WATTS";
        default:
            return "MEASURE_DC_AMPS";
    }
}

/**
 * @brief Helper function returning the sense enum as string which can be
 * used within the TCP command send to the SMU.
 * @param sense sense enum to convert to a string
 * @return autoZero as string.
 */
/*static*/ std::string KEI2600::getStringFromSenseValue(SMU::SMU_SENSE sense) {
    switch (sense) {
        case LOCAL:
            return "SENSE_LOCAL";
        case REMOTE:
            return "SENSE_REMOTE";
        case CALIBRATION:
            return "SENSE_CALA";
        default:
            return "SENSE_LOCAL";
    }
}

std::string KEI2600::getLetterFromUnit(UNIT unit) {
    switch (unit) {
        case SMU::CURRENT:
            return "i";
        case SMU::VOLTAGE:
            return "v";
        case SMU::RESISTANCE:
            return "r";
        case SMU::POWER:
            return "p";
        default:
            return "";
    }
}

std::string KEI2600::getMeasurementBufferName(SMU_CHANNEL channel) {
    std::string prefix = channel == CHANNEL_A ? "A" : "B";
    return prefix + "_M_BUFFER";
}

/**
 * @brief Determines whether the value to be measured should be stored in a buffer or not. If it should be saved, the
 * number of buffer entries is incremented.
 * @param channel The channel on which to measure.
 * @return An empty string if the measurement is not buffered, the buffer to save it in otherwise.
 */
std::string KEI2600::getMeasurementStorage(SMU_CHANNEL channel) {
    if (!isBuffered()) {
        return "";
    } else {
        if (channel == CHANNEL_A)
            m_bufferEntriesA++;
        else
            m_bufferEntriesB++;
        return getMeasurementBufferName(channel);
    }
}
