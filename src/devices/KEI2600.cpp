/**
 * @authors Wuhao Lius, Florian Frank
 */
#include "iostream"
#include "devices/KEI2600.h"

using namespace std;

KEI2600::KEI2600(const char *ip, int timeoutInMS, PIL::Logging *logger) : SMU(ip, timeoutInMS, logger) {
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


PIL_ERROR_CODE KEI2600::turnOn(SMU_CHANNEL channel) {
    auto channelStr = getChannelStringFromEnum(channel);
    return Exec("smu" + channelStr + ".source.output = smu" + channelStr + ".OUTPUT_ON");
}


PIL_ERROR_CODE KEI2600::turnOff(SMU_CHANNEL channel) {
    auto channelStr = getChannelStringFromEnum(channel);
    return Exec("smu" + channelStr + ".source.output = smu" + channelStr + ".OUTPUT_OFF");
}


PIL_ERROR_CODE KEI2600::setLevel(UNIT unit, SMU_CHANNEL channel, double level) {
    string sValue = to_string(level);
    string levelType;
    switch (unit)
    {
        case CURRENT:
            levelType = ".source.leveli = ";
            break;
        case VOLTAGE:
            levelType = ".source.levelv = ";
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }

    string command = "smu" + getChannelStringFromEnum(channel) + levelType + sValue;
    return Exec(command);
}


PIL_ERROR_CODE KEI2600::setLimit(UNIT unit, SMU_CHANNEL channel, double limit) {
    string sValue = to_string(limit);
    string limitType;
    switch (unit)
    {
        case CURRENT:
            limitType = ".source.limiti = ";
            break;
        case VOLTAGE:
            limitType = ".source.limitv = ";
            break;
        case POWER:
            limitType = ".source.limitp = ";
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }
    string command = "smu" + getChannelStringFromEnum(channel) + limitType + sValue;
    return Exec(command);
}


PIL_ERROR_CODE KEI2600::enableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel) {
    string measureType;
    switch (unit)
    {
        case CURRENT:
            measureType = ".measure.autorangei = smu";
            break;
        case VOLTAGE:
            measureType = ".measure.autorangev = smu";
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }
    auto channelStr = getChannelStringFromEnum(channel);
    return Exec("smu" + channelStr + measureType + channelStr + ".AUTORANGE_ON");
}


PIL_ERROR_CODE KEI2600::disableMeasureAutoRange(UNIT unit, SMU_CHANNEL channel) {
    string measureType;
    switch (unit)
    {
        case CURRENT:
            measureType = ".measure.autorangei = smu";
            break;
        case VOLTAGE:
            measureType = ".measure.autorangev = smu";
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }
    return Exec("smu" + getChannelStringFromEnum(channel) + measureType + getChannelStringFromEnum(channel) +
                ".AUTORANGE_OFF");
}


PIL_ERROR_CODE KEI2600::enableSourceAutoRange(UNIT unit, SMU_CHANNEL channel) {
    string sourceType;
    switch (unit)
    {
        case CURRENT:
            sourceType = ".source.autorangei = smu";
            break;
        case VOLTAGE:
            sourceType = ".source.autorangei = smu";
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }
    auto channelStr = getChannelStringFromEnum(channel);
    return Exec("smu" + channelStr + sourceType + channelStr + ".AUTORANGE_ON");
}


PIL_ERROR_CODE KEI2600::disableSourceAutoRange(UNIT unit, SMU_CHANNEL channel) {
    string sourceType;
    switch (unit)
    {
        case CURRENT:
            sourceType = ".source.autorangei = smu";
            break;
        case VOLTAGE:
            sourceType = ".source.autorangei = smu";
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }

    auto channelStr = getChannelStringFromEnum(channel);
    return Exec("smu" + channelStr + sourceType + channelStr + ".AUTORANGE_ON");
}


/**
 * @brief  this attribute returns the positive full-scale value of the measurement range that the SMU is
 * currently using. Assigning a value to this attribute sets the SMU on a fixed range large enough to
 * measure the assigned value. The instrument selects the best range for measuring a value of rangeValue
 * */
PIL_ERROR_CODE KEI2600::setMeasureRange(UNIT unit, SMU_CHANNEL channel, double rangeValue) {
    string measureRange;
    string sValue = to_string(rangeValue);
    switch (unit)
    {
        case CURRENT:
            measureRange = ".measure.rangei = ";
            break;
        case VOLTAGE:
            measureRange = ".measure.rangev = ";
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }
    return Exec("smu" + getChannelStringFromEnum(channel) + measureRange + sValue);
}


PIL_ERROR_CODE KEI2600::setSourceRange(UNIT unit, SMU_CHANNEL channel, double rangeValue) {
    string sValue = to_string(rangeValue);
    string rangeType;
    switch (unit)
    {
        case CURRENT:
            rangeType = ".source.rangei = ";
            break;
        case VOLTAGE:
            rangeType = ".source.rangev = ";
            break;
        default:
            return PIL_INVALID_ARGUMENTS;
    }
    return Exec("smu" + getChannelStringFromEnum(channel) + rangeType + sValue);
}


PIL_ERROR_CODE KEI2600::selectLocalSense(SMU_CHANNEL channel) {
    auto channelStr = getChannelStringFromEnum(channel);
    return Exec("smu" + channelStr + ".sense = smu" + channelStr + ".SENSE_LOCAL");
}


PIL_ERROR_CODE KEI2600::selectRemoteSense(SMU_CHANNEL channel) {
    auto channelStr = getChannelStringFromEnum(channel);
    return Exec("smu" + channelStr + ".sense = smu" + channelStr + ".SENSE_REMOTE");
}


PIL_ERROR_CODE KEI2600::enableBeep() {
    return Exec("beeper.enable = beeper.ON");
}


PIL_ERROR_CODE KEI2600::beep() {
    return Exec("beeper.beep(1, 2400)");
}


PIL_ERROR_CODE KEI2600::disableBeep() {
    return Exec("beeper.enable = beeper.OFF");
}


PIL_ERROR_CODE KEI2600::measureI(SMU_CHANNEL channel, double *value)
{
    if(!value)
        return PIL_INVALID_ARGUMENTS;

    string command = "reading = smu" + getChannelStringFromEnum(channel) + ".measure.i()";
    auto ret = Exec(command);
    if(ret != PIL_NO_ERROR)
        return ret;

    char buffer[1024] = {0};
    string response = "print(reading)";
    ret = Exec(response, buffer);
    if(ret != PIL_NO_ERROR)
        return ret;

    cout << "Received from measureI: " << buffer << endl; // TODO replace by logging function
    *value = stod(buffer);
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::measureV(SMU_CHANNEL channel, double *value)
{
    if(!value)
        return PIL_INVALID_ARGUMENTS;

    string command = "reading = smu" + getChannelStringFromEnum(channel) + ".measure.v()";
    auto ret = Exec(command);
    if (ret != PIL_NO_ERROR)
        return ret;

    char buffer[1024] = "0";
    string response = "print(reading)";
    ret = Exec(response, buffer);
    if (ret != PIL_NO_ERROR)
        return ret;


    cout << "Received from measureV: " << buffer << endl; // TODO replace by logging
    *value = stod(buffer);
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::measureR(SMU_CHANNEL channel, double *value)
{
    if(!value)
        return PIL_INVALID_ARGUMENTS;

    string command = "reading = smu" + getChannelStringFromEnum(channel) + ".measure.r()";
    char buffer[1024] = {0};
    auto ret = Exec(command, buffer);
    if (ret != PIL_NO_ERROR)
        return ret;

    *value = stod(buffer);
    return PIL_NO_ERROR;
}


PIL_ERROR_CODE KEI2600::measureP(SMU_CHANNEL channel, double *value)
{
    if(!value)
        return PIL_INVALID_ARGUMENTS;

    string command = "reading = smu" + getChannelStringFromEnum(channel) + ".measure.p()";
    char buffer[1024] = {0};
    auto ret = Exec(command, buffer);
    if (ret != PIL_NO_ERROR)
        return ret;

    *value = stod(buffer);
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
    }
}
