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


PIL_ERROR_CODE KEI2600::measure(UNIT unit, SMU_CHANNEL channel, double* value)
{
    switch (unit)
    {
        case SMU::CURRENT:
            return measureI(channel, value);
        case SMU::VOLTAGE:
            return measureV(channel, value);
        case SMU::RESISTANCE:
            return measureR(channel, value);
        case SMU::POWER:
            return measureP(channel, value);
        default:
            return PIL_INVALID_ARGUMENTS;
    }
}


double KEI2600::measurePy(UNIT unit, SMU_CHANNEL channel)
{
    double value;
    measure(unit, channel, &value);
    return value;
}

PIL_ERROR_CODE KEI2600::turnOn(SMU_CHANNEL channel)
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

    return Exec("", &execArgs);
}


PIL_ERROR_CODE KEI2600::turnOff(SMU_CHANNEL channel)
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

    return Exec("", &execArgs);
}

/**
 * @brief smuA.source.leveli = 3
 * @param unit
 * @param channel
 * @param level
 * @return
 */
PIL_ERROR_CODE KEI2600::setLevel(UNIT unit, SMU_CHANNEL channel, double level)
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

    return Exec("", &args);
}


PIL_ERROR_CODE KEI2600::setLimit(UNIT unit, SMU_CHANNEL channel, double limit)
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

    return Exec("", &args);
}


PIL_ERROR_CODE KEI2600::enableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel)
{
    SubArg subArg("");
    subArg.AddElem("measure", ".");

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

   // auto channelStr = getChannelStringFromEnum(channel);
    return Exec("", &args);
}

PIL_ERROR_CODE KEI2600::disableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel)
{
    SubArg subArg("");
    subArg.AddElem("measure", ".");

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
}


PIL_ERROR_CODE KEI2600::enableSourceAutoRange(UNIT unit, SMU_CHANNEL channel)
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
}


PIL_ERROR_CODE KEI2600::disableSourceAutoRange(UNIT unit, SMU_CHANNEL channel)
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
}


PIL_ERROR_CODE KEI2600::enableMeasureAnalogFilter(SMU_CHANNEL smuChannel)
{
    SubArg subArg(getChannelStringFromEnum(smuChannel), ".");
    subArg.AddElem("measure", ".")
    .AddElem("analogfilter");

    ExecArgs arg;
    arg.AddArgument(subArg, "1", " = ");

    return Exec("", &arg);
}


PIL_ERROR_CODE KEI2600::disableMeasureAnalogFilter(SMU_CHANNEL smuChannel)
{
    SubArg subArg(getChannelStringFromEnum(smuChannel), ".");
    subArg.AddElem("measure", ".")
            .AddElem("analogfilter");

    ExecArgs arg;
    arg.AddArgument(subArg, "0", " = ");

    return Exec("", &arg);
}


/**
 * @brief  this attribute returns the positive full-scale value of the measurement range that the SMU is
 * currently using. Assigning a value to this attribute sets the SMU on a fixed range large enough to
 * measure the assigned value. The instrument selects the best range for measuring a value of rangeValue
 * */
PIL_ERROR_CODE KEI2600::setMeasureRange(UNIT unit, SMU_CHANNEL channel, double rangeValue)
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

    return Exec("", &args);
}


PIL_ERROR_CODE KEI2600::setSourceRange(UNIT unit, SMU_CHANNEL channel, double rangeValue)
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

    return Exec("", &args);
}


PIL_ERROR_CODE KEI2600::selectLocalSense(SMU_CHANNEL channel)
{
    SubArg subArg("sense", ".");
    SubArg smuArg("smu");
    smuArg.AddElem(getChannelStringFromEnum(channel));
    SubArg localSenseArg("SENSE_LOCAL", ".");

    ExecArgs execArgs;
             execArgs.AddArgument("smu", getChannelStringFromEnum(channel))
             .AddArgument(subArg, smuArg, " = ")
             .AddArgument(localSenseArg, "");

    return Exec("", &execArgs);
}


PIL_ERROR_CODE KEI2600::selectRemoteSense(SMU_CHANNEL channel)
{
    SubArg subArg("sense", ".");
    SubArg smuArg("smu");
    smuArg.AddElem(getChannelStringFromEnum(channel));
    SubArg remoteSenseArg("SENSE_REMOTE", ".");

    ExecArgs args;
    args.AddArgument("smu", getChannelStringFromEnum(channel))
        .AddArgument(subArg, smuArg, " = ")
        .AddArgument(remoteSenseArg, "");

    return Exec("", &args);
}

/**
 * @brief Sets the integration apeture for measurements.
 * @param channel channel which should be set
 * @return error code
 */
PIL_ERROR_CODE KEI2600::setMeasurePLC(SMU_CHANNEL channel, double value)
{
    if(value < 0.001 || value > 25)
        return PIL_INVALID_ARGUMENTS;

    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
    .AddElem("measure", ".")
    .AddElem("nplc", ".");

    ExecArgs args;
    args.AddArgument(subArg, value, " = ");

    return Exec("", &args);
}

/**
 * @brief Sets the lowest measurement range that is used when the instrument is autoranging.
 * @param unit low range unit to set. Allowed are Current and voltage.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param value value to set.
 * @return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasureLowRange(UNIT unit, SMU_CHANNEL channel, double value)
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

    return Exec("", &execArgs);
}

/**
 * @brief Enables or disableds automatic updates to the internal reference measurements (autozero) of the instrument.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param autoZero can be set either to OFF, AUTO or ONCE. (See AUTOZERO enum)
 * @return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasureAutoZero(SMU_CHANNEL channel, AUTOZERO autoZero)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
    .AddElem("measure", ".")
    .AddElem("autozero", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu"+ getChannelStringFromEnum(channel)+"."+getStringFromAutoZeroEnum(autoZero), " = ");

    return Exec("", &arg);
}


/**
 * @brief Sets the number of measurements made when a measurement is requested.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param nrOfMeasurements number of measurements made.
 * @return error code.
 */
PIL_ERROR_CODE KEI2600::setMeasureCount(SMU_CHANNEL channel, int nrOfMeasurements)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("count", ".");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, nrOfMeasurements, " = ");

    return Exec("", &execArgs);
}

/**
 * @brief Set the source function which can bei either voltage or current.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param srcFunc Voltage or Current function to set.
 * @return error code.
 */
PIL_ERROR_CODE KEI2600::setSourceFunction(SMU_CHANNEL channel, SRC_FUNC srcFunc)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("func", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu"+ getChannelStringFromEnum(channel)+"."+getStringFromSrcFuncEnum(srcFunc), " = ");

    return Exec("", &arg);
}

/**
 * @brief Sets the source output-off mode
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param offMode can be either a OUTPUT_NORMAL defined by offfunc, ZERO or HIGH_Z.
 * @return error code
 */
PIL_ERROR_CODE KEI2600::setSourceOffMode(SMU_CHANNEL channel, SRC_OFF_MODE offMode)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("offmode", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu" + getChannelStringFromEnum(channel) + "." + getStringFromOffModeEnum(offMode), " = ");

    return Exec("", &arg);
}


/**
 * @brief Set the source settling mode. (See SRC_SETTLING enum for more information)
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param srcSettling settling mode. (See SRC_SETTLING enum for more information)
 * @return error code.
 */
PIL_ERROR_CODE KEI2600::setSourceSettling(SMU::SMU_CHANNEL channel, SRC_SETTLING srcSettling)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("settling", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "smu" + getChannelStringFromEnum(channel) + "." + getStringFromSettleEnum(srcSettling), " = ");

    return Exec("", &arg);
}

/**
 * @brief Turns on the source sink mode.
 * This reduces the source limit inaccuracy that occurs when operating in quadrants II and IV
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @return error code.
 */
PIL_ERROR_CODE KEI2600::enableSourceSink(SMU_CHANNEL channel)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("sink", ".");

    ExecArgs arg;
    arg.AddArgument(subArg, "1", " = ");

    return Exec("", &arg);
}

/**
 * @brief Turns off the source sink mode.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @return error code.
 */
PIL_ERROR_CODE KEI2600::disableSourceSink(SMU_CHANNEL channel)
{
    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
            .AddElem("source", ".")
            .AddElem("sink", ".");

    ExecArgs arg;
    arg.AddArgument(subArg, "0", " = ");

    return Exec("", &arg);
}

/**
 * @brief Specify the type of measurement currently displayed.
 * @param channel channel on which this operation should be applied (SMU_CHANNEL_A, SMU_CHANNEL_B)
 * @param measureFunc show amps, volts, ohms or watts.
 * @return error code.
 */
PIL_ERROR_CODE KEI2600::displayMeasureFunction(SMU::SMU_CHANNEL channel, DISPLAY_MEASURE_FUNC measureFunc)
{
    SubArg subArg("display");
    subArg.AddElem("smu")
    .AddElem(getChannelStringFromEnum(channel))
            .AddElem("measure", ".")
            .AddElem("func", ".");

    ExecArgs arg;
    // TODO avoid concatination
    arg.AddArgument(subArg, "display" + std::string(".") +
    getStringFromMeasureDisplayFunction(measureFunc), " = ");

    return Exec("", &arg);
}


PIL_ERROR_CODE KEI2600::enableBeep()
{
    SubArg subArg("beeper");
    subArg.AddElem("enable", ".");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, "beeper.ON", " = ");

    return Exec("", &execArgs);
}


PIL_ERROR_CODE KEI2600::beep()
{
    return Exec("beeper.beep(1, 2400)");
}


PIL_ERROR_CODE KEI2600::disableBeep()
{
    SubArg subArg("beeper");
    subArg.AddElem("enable", ".");

    SubArg beeperOffArg("beeper");
           beeperOffArg.AddElem("OFF", ".");

    ExecArgs execArgs;
    execArgs.AddArgument(subArg, beeperOffArg, " = ");

    return Exec("", &execArgs);
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
}

/*static*/ std::string KEI2600::getStringFromSettleEnum(SRC_SETTLING srcSettling)
{
    switch (srcSettling)
    {
        case SETTLING_SMOOTH:
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
}

/*static*/ std::string KEI2600::getStringFromMeasureDisplayFunction(DISPLAY_MEASURE_FUNC displayMeausreFunc)
{
    switch (displayMeausreFunc)
    {

        case MEASURE_DC_AMPS:
            return "MEASURE_DC_AMPS";
        case MEASURE_DC_VOLTS:
            return "MEASURE_DC_VOLTS";
        case MEASURE_OHMS:
            return "MEASURE_OHMS";
        case MEASURE_WATTS:
            return "MEASURE_WATTS";
    }
}
