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


PIL_ERROR_CODE KST33500::display(std::string &text)
{
    SubArg arg("DISP");
    arg.AddElem("TEXT", ":");

    ExecArgs args;
    args.AddArgument(arg, "'" + text + "'", " ");

    return Exec("", &args);
}

// TODO not needed I guess
PIL_ERROR_CODE KST33500::displayConnection() {
    auto execRet = Exec("DISP:TEXT 'Connected Successfully. Returning...'");
    if(execRet != PIL_NO_ERROR)
        return execRet;

  sleep(2);
    execRet = Exec("DISP ON");
    if(execRet != PIL_NO_ERROR)
        return execRet;

    return Exec("DISPlay:TEXT:CLEar");
}


/**
 * argument unit: ms
 * */
PIL_ERROR_CODE KST33500::setPulseWidth(double value) {
  string msg = "FUNCtion:PULSe:WIDTh ";

  SubArg arg("FUNCtion");
  arg.AddElem("PULSe", ":")
  .AddElem("WIDTh", ":");

  ExecArgs args;
  args.AddArgument(arg, msg + " ms", " ");

  return Exec("", &args);
}

PIL_ERROR_CODE KST33500::output(bool on) {
  ExecArgs args;
  if (on)
    args.AddArgument("ON", " ");
  else
      args.AddArgument("OFF", " ");

    return Exec("OUTPut", &args);
}

PIL_ERROR_CODE KST33500::turnOn(FunctionGenerator::FUNC_CHANNEL channel)
{
    return output(true);
}

PIL_ERROR_CODE KST33500::turnOff(FunctionGenerator::FUNC_CHANNEL channel)
{
    return output(false);
}

PIL_ERROR_CODE KST33500::setFrequency(FunctionGenerator::FUNC_CHANNEL channel, double value)
{
    string msg = "FREQuency " + to_string(value);
    ExecArgs args;
    args.AddArgument("", value);
    return Exec("FREQuency ", &args);
}

// TODO unclear
PIL_ERROR_CODE KST33500::setAmplitude(FunctionGenerator::FUNC_CHANNEL channel, double value, const char *constrain)
{
    std::string msg = "VOLTage";
    if (strlen(constrain) > 0) {
        msg += ":" + std::string(constrain) + " ";
    }
    msg += " " + std::to_string(value);
    auto execRet = Exec(msg);
    if(execRet != PIL_NO_ERROR)
        return execRet;

    m_Amplitude = value;
    return PIL_NO_ERROR;
}


/**
 * @brief Set output function of the function generator. E.g. SIN sets a sinus wave.
 * @param functionType type of output function to set. E.g. SINUS or SQUARE for sinus and square waves
 * @return true if no error occurs. Otherwise the error could be read by calling return_error_message().
 */
PIL_ERROR_CODE KST33500::setFunction(FunctionGenerator::FUNCTION_TYPE functionType)
{
    auto function = GetFunctionStr(functionType);


    ExecArgs args;
    args.AddArgument("FUNCtion", function, " ");

    auto execRet = Exec("", &args);
    if(execRet != PIL_NO_ERROR)
        return execRet;

    m_CurrentFunction = functionType;
    return PIL_NO_ERROR;
}

PIL_ERROR_CODE KST33500::setPhase(FunctionGenerator::FUNC_CHANNEL channel, double value)
{
    ExecArgs args;
    args.AddArgument("PHASe", value, " ");
    return Exec("", &args);
}

/**
 * argument unit: V
 * attention: offset would change voltage by double of your argument
 * */
PIL_ERROR_CODE KST33500::setOffset(FunctionGenerator::FUNC_CHANNEL channel, double offset)
{
    SubArg voltageOffset("VOLTage");
    voltageOffset.AddElem("OFFSet", ":");

    ExecArgs args;
    args.AddArgument(voltageOffset, offset, " ");
    return Exec("", &args);
}

std::string KST33500::GetFunctionStr(FUNCTION_TYPE functionType)
{
    switch (functionType)
    {
        case SIN:
            return "SIN";
        case SQUARE:
            return "SQU";
        case RAMP:
            return "RAMP";
        case NEGATIVE_RAM:
            return "NRAM";
        case TRIANGLE:
            return "TRI";
        case NOISE:
            return "NOIS";
        case PSEUDO_RANDOM_BIT_STREAM:
            return "PRBS";
        case ARBITRARY:
            return "ARB";
        default:
            return "SIN";
    }
}
