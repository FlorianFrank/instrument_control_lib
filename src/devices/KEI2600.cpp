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
 * @return
 */
PIL_ERROR_CODE KEI2600::setMeasurePLC(SMU_CHANNEL channel, double value)
{
    if(value < 0.001 || value > 25)
        return PIL_INVALID_ARGUMENTS;

    SubArg subArg("smu");
    subArg.AddElem(getChannelStringFromEnum(channel))
    .AddElem("nplc", ".");

    ExecArgs args;
    args.AddArgument(subArg, value, " = ");

    return Exec("", &args);

    return PIL_ERRNO;
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

