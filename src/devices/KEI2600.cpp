/**
 * @authors Wuhao Lius, Florian Frank
 */
#include "devices/KEI2600.h"
#include "ctlib/Logging.hpp"

KEI2600::KEI2600(const char *ip, int timeoutInMS, PIL::Logging *logger) : SMU(ip, timeoutInMS, logger) {
    this->m_DeviceName = DEVICE_NAME;
}

KEI2600::KEI2600(const char *ip, int timeoutInMS) : SMU(ip, timeoutInMS, nullptr) {
    this->m_DeviceName = DEVICE_NAME;
}


PIL_ERROR_CODE KEI2600::measure(UNIT unit, SMU_CHANNEL channel, double *value, bool checkErrorBuffer)
{
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


double KEI2600::measurePy(UNIT unit, SMU_CHANNEL channel)
{
    double value;
    measure(unit, channel, &value, true);
    return value;
}

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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief smuA.source.leveli = 3
 * @param unit
 * @param channel
 * @param level
 * @return
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::enableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer)
{
    auto ret = measureAutoRangeHelperFunction(channel, unit, true);
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

PIL_ERROR_CODE KEI2600::disableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel, bool checkErrorBuffer)
{
    auto ret = measureAutoRangeHelperFunction(channel, unit, false);
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;

}


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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;

}


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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::enableMeasureAnalogFilter(SMU_CHANNEL smuChannel, bool checkErrorBuffer)
{
    auto ret = analogFilterHelperFunction(smuChannel, true);
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::disableMeasureAnalogFilter(SMU_CHANNEL smuChannel, bool checkErrorBuffer)
{
    auto ret = analogFilterHelperFunction(smuChannel, false);
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


/**
 * @brief  this attribute returns the positive full-scale value of the measurement range that the SMU is
 * currently using. Assigning a value to this attribute sets the SMU on a fixed range large enough to
 * measure the assigned value. The instrument selects the best range for measuring a value of rangeValue
 * */
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::selectLocalSense(SMU_CHANNEL channel, bool checkErrorBuffer)
{
    SubArg subArg("sense", ".");
    SubArg smuArg("smu");
    smuArg.AddElem(getChannelStringFromEnum(channel));
    SubArg localSenseArg("SENSE_LOCAL", ".");

    ExecArgs execArgs;
             execArgs.AddArgument("smu", getChannelStringFromEnum(channel))
             .AddArgument(subArg, smuArg, " = ")
             .AddArgument(localSenseArg, "");

    auto ret = Exec("", &execArgs);
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::selectRemoteSense(SMU_CHANNEL channel, bool checkErrorBuffer)
{
    SubArg subArg("sense", ".");
    SubArg smuArg("smu");
    smuArg.AddElem(getChannelStringFromEnum(channel));
    SubArg remoteSenseArg("SENSE_REMOTE", ".");

    ExecArgs args;
    args.AddArgument("smu", getChannelStringFromEnum(channel))
        .AddArgument(subArg, smuArg, " = ")
        .AddArgument(remoteSenseArg, "");

    auto ret = Exec("", &args);
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the integration apeture for measurements.
 * @param channel channel which should be set
 * @return error code
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
    if(ret != PIL_NO_ERROR)
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
 * @return error code.
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;

}

/**
 * @brief Enables or disableds automatic updates to the internal reference measurements (autozero) of the instrument.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param autoZero can be set either to OFF, AUTO or ONCE. (See AUTOZERO enum)
 * @return error code.
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


/**
 * @brief Sets the number of measurements made when a measurement is requested.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param nrOfMeasurements number of measurements made.
 * @return error code.
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Set the source function which can bei either voltage or current.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param srcFunc Voltage or Current function to set.
 * @return error code.
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Sets the source output-off mode
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param offMode can be either a OUTPUT_NORMAL defined by offfunc, ZERO or HIGH_Z.
 * @return error code
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


/**
 * @brief Set the source settling mode. (See SRC_SETTLING enum for more information)
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param srcSettling settling mode. (See SRC_SETTLING enum for more information)
 * @return error code.
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Turns on the source sink mode.
 * This reduces the source limit inaccuracy that occurs when operating in quadrants II and IV
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @return error code.
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

/**
 * @brief Turns off the source sink mode.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @return error code.
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}

#include "iostream"
/**
 * @brief Specify the type of measurement currently displayed.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param measureFunc show amps, volts, ohms or watts.
 * @return error code.
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
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::enableBeep(bool checkErrorBuffer)
{
    SubArg subArg("beeper");
    subArg.AddElem("enable", ".");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, "beeper.ON", " = ");

    auto ret = Exec("", &execArgs);
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::beep(float time, int frequency, bool checkErrorBuffer)
{
    auto ret = Exec("beeper.beep("+std::to_string(time)+","+std::to_string(frequency) +")");
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::disableBeep(bool checkErrorBuffer)
{
    SubArg subArg("beeper");
    subArg.AddElem("enable", ".");

    SubArg beeperOffArg("beeper");
           beeperOffArg.AddElem("OFF", ".");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, beeperOffArg, " = ");

    auto ret = Exec("", &execArgs);
    if(ret != PIL_NO_ERROR)
        return ret;
    if(checkErrorBuffer)
        return getErrorBufferStatus();
    return PIL_NO_ERROR;
}


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
    if (ret != PIL_NO_ERROR)
        return ret;

    char buffer[MEASURE_RET_BUFF_SIZE] = {0};

    SubArg subArgPrint("");
    subArgPrint.AddElem("reading", "(", ")");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, "");

    ret = Exec("print", &execArgs, buffer);
    if (ret != PIL_NO_ERROR)
        return ret;

    if(m_Logger)
        m_Logger->LogMessage(DEBUG_LVL, __FILENAME__, __LINE__,"measureI returned: %s", buffer);
    *value = std::stod(buffer);
    return PIL_NO_ERROR;
}


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
    if (ret != PIL_NO_ERROR)
        return ret;

    char buffer[MEASURE_RET_BUFF_SIZE] = {0};

    SubArg subArgPrint("");
    subArgPrint.AddElem("reading", "(", ")");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, "");

    ret = Exec("print", &execArgs, buffer);
    if (ret != PIL_NO_ERROR)
        return ret;

    if(m_Logger)
        m_Logger->LogMessage(DEBUG_LVL, __FILENAME__, __LINE__,"measureV returned: %s", buffer);
    *value = std::stod(buffer);
    return PIL_NO_ERROR;
}


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
    if (ret != PIL_NO_ERROR)
        return ret;

    char buffer[MEASURE_RET_BUFF_SIZE] = {0};

    SubArg subArgPrint("");
    subArgPrint.AddElem("reading", "(", ")");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, "");

    ret = Exec("print", &execArgs, buffer);
    if (ret != PIL_NO_ERROR)
        return ret;

    if(m_Logger)
        m_Logger->LogMessage(DEBUG_LVL, __FILENAME__, __LINE__,"measureR returned: %s", buffer);
    *value = std::stod(buffer);
    return PIL_NO_ERROR;
}


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
    if (ret != PIL_NO_ERROR)
        return ret;

    char buffer[MEASURE_RET_BUFF_SIZE] = {0};

    SubArg subArgPrint("");
    subArgPrint.AddElem("reading", "(", ")");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, "");

    ret = Exec("print", &execArgs, buffer);
    if (ret != PIL_NO_ERROR)
        return ret;

    if(m_Logger)
        m_Logger->LogMessage(DEBUG_LVL, __FILENAME__, __LINE__,"measureP returned: %s", buffer);
    *value = std::stod(buffer);
    return PIL_NO_ERROR;
}

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

/*static*/ std::string KEI2600::getStringFromMeasureDisplayFunction(SMU_DISPLAY displayMeausreFunc)
{
    switch (displayMeausreFunc){

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

#include "iostream"
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

    // auto channelStr = getChannelStringFromEnum(channel);
    return Exec("", &args);
}

std::string KEI2600::getLastError()
{
    ExecArgs argsErrorQueue;
    argsErrorQueue.AddArgument("errorcode, message = errorqueue.next()", "");
    auto ret = Exec("", &argsErrorQueue);
    if(ret != PIL_NO_ERROR)
        return "INTERNAL ERROR";

    char errorBuffer[2048];
    ExecArgs argsPrintError;
    argsPrintError.AddArgument("print(errorcode, message)", "");
    ret = Exec("", &argsPrintError, errorBuffer, true, 2048);
    if(ret != PIL_NO_ERROR)
        return "INTERNAL ERROR";
    std::string  retStr = std::string(errorBuffer);
    return retStr.substr(0, retStr.find('\n'));
}

PIL_ERROR_CODE KEI2600::clearErrorBuffer()
{
    ExecArgs args;
    args.AddArgument("errorqueue", "clear()", ".");

    return Exec("", &args);
}

#include "iostream"
PIL_ERROR_CODE KEI2600::getErrorBufferStatus()
{
    ExecArgs argsErrorQueue;
    argsErrorQueue.AddArgument("count = errorqueue.count", "");
    auto ret = Exec("", &argsErrorQueue);
    if(ret != PIL_NO_ERROR)
        return ret;

    char errorBuffer[2048];
    ExecArgs argsPrintError;
    argsPrintError.AddArgument("print(count)", "");
    ret = Exec("", &argsPrintError, errorBuffer, true, 2048);
    if(ret != PIL_NO_ERROR)
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