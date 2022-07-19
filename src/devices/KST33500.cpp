//
// Created by liuwuhao on 27.05.21.
//

#include "devices/KST33500.h"
#include <unistd.h>
#include <cstring>

using namespace std;

KST33500::KST33500(const char *ip, int timeoutInMS) : FunctionGenerator(ip, timeoutInMS, nullptr){
  this->m_DeviceName = "Keysight 33500B Waveform Generator";
}

KST33500::KST33500(const char *ip, int timeoutInMs, PIL::Logging *logger) : FunctionGenerator(ip, timeoutInMs, logger)
{
    this->m_DeviceName = "Keysight 33500B Waveform Generator";
}



bool KST33500::display(std::string &text) {
  string msg = "DISP:TEXT '" + text + "'";
    Exec(msg);
  return 0;
}

bool KST33500::display_connection() {
    Exec("DISP:TEXT 'Connected Successfully. Returning...'");
  sleep(2);
    Exec("DISP ON");
    Exec("DISPlay:TEXT:CLEar");
  return 0;
}


/**
 * argument unit: V
 * attention: offset would change voltage by double of your argument
 * */
int KST33500::offset(double value) {
  string msg = "VOLTage:OFFSet ";
  msg += to_string(value);
  return Exec(msg);
}

/**
 * argument unit: ms
 * */
int KST33500::set_pulse_width(double value) {
  string msg = "FUNCtion:PULSe:WIDTh ";
  msg += to_string(value) + " ms";
  return Exec(msg);
}

bool KST33500::output(bool on) {
  string msg = "OUTPut";
  if (on) {
    msg += " ON";
  } else {
    msg += " OFF";
  }
    Exec(msg);
  return 0;
}

int KST33500::turnOn(FunctionGenerator::FUNC_CHANNEL channel)
{
    return output(true);
}

int KST33500::turnOff(FunctionGenerator::FUNC_CHANNEL channel)
{
    return output(false);
}

int KST33500::setFrequency(FunctionGenerator::FUNC_CHANNEL channel, double value)
{
    string msg = "FREQuency " + to_string(value);
    Exec(msg);
    return 0;
}

int KST33500::setAmplitude(FunctionGenerator::FUNC_CHANNEL channel, double value, const char *constrain)
{
    std::string msg = "VOLTage";
    if (strlen(constrain) > 0) {
        msg += ":" + std::string(constrain) + " ";
    }
    msg += " " + std::to_string(value);
    if(!Exec(msg))
        return false;

    m_Amplitude = value;
    return true;
}


/**
 * @brief Set output function of the function generator. E.g. SIN sets a sinus wave.
 * @param functionType type of output function to set. E.g. SINUS or SQUARE for sinus and square waves
 * @return true if no error occurs. Otherwise the error could be read by calling return_error_message().
 */
bool KST33500::setFunction(FunctionGenerator::FUNCTION_TYPE functionType)
{
    std::string function;
    switch (functionType)
    {
        case SIN:
            function = "SIN";
            break;
        case SQUARE:
            function = "SQU";
            break;
        case RAMP:
            function = "RAMP";
            break;
        case NEGATIVE_RAM:
            function = "NRAM";
            break;
        case TRIANGLE:
            function = "TRI";
            break;
        case NOISE:
            function = "NOIS";
            break;
        case PSEUDO_RANDOM_BIT_STREAM:
            function = "PRBS";
            break;
        case ARBITRARY:
            function = "ARB";
            break;
        default:
            function = "SIN";
    }
    std::string msg = "FUNCtion " + function;
    if(!Exec(msg))
        return false;

    m_CurrentFunction = functionType;
    return true;
}

int KST33500::setPhase(FunctionGenerator::FUNC_CHANNEL channel, double value)
{
    string msg = "PHASe " + std::to_string(value);
    Exec(msg);
    return false;
}

bool KST33500::setOffset(FunctionGenerator::FUNC_CHANNEL channel, double offset)
{
    return false;
}
