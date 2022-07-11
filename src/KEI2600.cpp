//
// Created by sueaki on 03.06.22.
//
#include "iostream"
#include "KEI2600.h"

using namespace std;

/**
 * @class KEI2600
 * @brief Keithley SMU 2600B
 */




KEI2600::KEI2600(PIL::Logging *logger, const char *ip, int timeoutInMS) : Device(ip, timeoutInMS, logger) {
    this->m_DeviceName = "Keithley SMU 2600B";
}

KEI2600::KEI2600(const char *ip, int timeoutInMS, PIL::Logging *logger) : Device(ip, timeoutInMS, logger) {
    this->m_DeviceName = "Keithley SMU 2600B";
}

int KEI2600::enableBeep() {
    return Exec("beeper.enable = beeper.ON");
}

int KEI2600::beep() {
    return Exec("beeper.beep(1, 2400)");
}

int KEI2600::disableBeep() {
    return Exec("beeper.enable = beeper.OFF");
}

int KEI2600::enableMeasureAutoRangeI(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".measure.autorangei = smu" + sChannel + ".AUTORANGE_ON");
}

int KEI2600::enableSourceAutoRangeI(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".source.autorangei = smu" + sChannel + ".AUTORANGE_ON");
}

int KEI2600::disableMeasureAutoRangeI(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".measure.autorangei = smu" + sChannel + ".AUTORANGE_OFF");
}

int KEI2600::disableSourceAutoRangeI(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".source.autorangei = smu" + sChannel + ".AUTORANGE_OFF");
}

int KEI2600::enableMeasureAutoRangeV(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".measure.autorangev = smu" + sChannel + " .AUTORANGE_ON");
}

int KEI2600::enableSourceAutoRangeV(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".source.autorangev = smu" + sChannel + " .AUTORANGE_ON");
}

int KEI2600::disableMeasureAutoRangeV(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".measure.autorangev = smu" + sChannel + " .AUTORANGE_OFF");
}

int KEI2600::disableSourceAutoRangeV(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".source.autorangev = smu" + sChannel + " .AUTORANGE_OFF");
}

/**
 * @brief  this attribute returns the positive full-scale value of the measurement range that the SMU is
 * currently using. Assigning a value to this attribute sets the SMU on a fixed range large enough to
 * measure the assigned value. The instrument selects the best range for measuring a value of rangeValue
 * */
int KEI2600::setMeasureRangeI(double rangeValue, SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string sValue = to_string(rangeValue);
    return Exec("smu" + sChannel + ".measure.rangei = " + sValue);
}

int KEI2600::setMeasureRangeV(double rangeValue, SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string sValue = to_string(rangeValue);
    return Exec("smu" + sChannel + ".measure.rangev = " + sValue);
}

int KEI2600::setSourceRangeI(double rangeValue, SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string sValue = to_string(rangeValue);
    return Exec("smu" + sChannel + ".source.rangei = " + sValue);
}

int KEI2600::setSourceRangeV(double rangeValue, SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string sValue = to_string(rangeValue);
    return Exec("smu" + sChannel + ".source.rangev = " + sValue);
}

double KEI2600::readI(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string command = "reading = smu" + sChannel + ".measure.i()";
    char buffer[1024] = {0};
    Exec(command, buffer);
    return stod(buffer);
}

double KEI2600::readV(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string command = "reading = smu" + sChannel + ".measure.v()";
    Exec(command);
    char buffer[1024] = "0";
    string response = "print(reading)";
    Exec(response, buffer);
    cout << "Received from readV: " << buffer << endl;
    return stod(buffer);
}

double KEI2600::readR(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string command = "reading = smu" + sChannel + ".measure.r()";
    char buffer[1024] = {0};
    Exec(command, buffer);
    return stod(buffer);
}


double KEI2600::readP(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string command = "reading = smu" + sChannel + ".measure.p()";
    char buffer[1024] = {0};
    Exec(command, buffer);
    return stod(buffer);
}

int KEI2600::setLevelI(double levelI, SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string sValue = to_string(levelI);
    string command = "smu" + sChannel + ".source.leveli = " + sValue;
    return Exec(command);
}

int KEI2600::setLevelV(double levelV, SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string sValue = to_string(levelV);
    string command = "smu" + sChannel + ".source.levelv = " + sValue;
    return Exec(command);
}


int KEI2600::setLimitI(double limitI, SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string sValue = to_string(limitI);
    string command = "smu" + sChannel + ".source.limiti = " + sValue;
    return Exec(command);
}

int KEI2600::setLimitV(double limitV, SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string sValue = to_string(limitV);
    string command = "smu" + sChannel + ".source.limitv = " + sValue;
    return Exec(command);
}


int KEI2600::setLimitP(double limitP, SMU_CHANNEL channel) {
    string sChannel(1, channel);
    string sValue = to_string(limitP);
    string command = "smu" + sChannel + ".source.limitp = " + sValue;
    return Exec(command);
}

int KEI2600::turnOn(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".source.output = smu" + sChannel + ".OUTPUT_ON");
}

int KEI2600::turnOff(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".source.output = smu" + sChannel + ".OUTPUT_OFF");
}

int KEI2600::selectLocalSense(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".sense = smu" + sChannel + ".SENSE_LOCAL");
}

int KEI2600::selectRemoteSense(SMU_CHANNEL channel) {
    string sChannel(1, channel);
    return Exec("smu" + sChannel + ".sense = smu" + sChannel + ".SENSE_REMOTE");
}

