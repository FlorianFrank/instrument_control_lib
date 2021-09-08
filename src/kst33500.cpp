//
// Created by liuwuhao on 27.05.21.
//

#include "kst33500.h"

#include <unistd.h> // sleep
#include <cstring> // strlen


KST33500::KST33500(const char *ip) : Device(ip), m_CurrentFunction(SIN), m_OutputEnabled(false), m_Amplitude(0) {
  this->m_DeviceName = "Keysight 33500B Waveform Generator";
}

/**
 * @brief Displays a passed  text connected successfully on the instrument front pannel.
 * @param text
 * @return
 */
bool KST33500::display(std::string &text) {
  std::string msg = "DISP:TEXT '" + text + "'";
  return exec(msg);
}

/**
 * @brief Displays the text 'connected successfully' on the instrument front pannel.
 * @return true if no error occurs. Otherwise the error could be read by calling return_error_message().
 */
bool KST33500::display_connection() {
  exec("DISP:TEXT 'Connected Successfully. Returning...'");
  sleep(2);
  if(!exec("DISP ON"))
      return false;
  return exec("DISPlay:TEXT:CLEar");
}

/**
 * @brief Set output function of the function generator. E.g. SIN sets a sinus wave.
 * @param functionType type of output function to set. E.g. SINUS or SQUARE for sinus and square waves
 * @return true if no error occurs. Otherwise the error could be read by calling return_error_message().
 */
bool KST33500::setFunction(FunctionType functionType)
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
    if(!exec(msg))
        return false;

    m_CurrentFunction = functionType;
    return true;
}

/**
 * @brief Set the frequency of the function generator.
 * @param value the frequency to set in Hz.
 * @return true if no error occurs. Otherwise the error could be read by calling return_error_message().
 */
bool KST33500::frequency(double value) {
  std::string msg = "FREQuency " + std::to_string(value);
  return exec(msg);
}

/**
 * @brief
 * @param value
 * @param constrain TODO whats the reason for that?
 * @return
 */
bool KST33500::voltage(double value, const char *constrain) {
  std::string msg = "VOLTage";
  if (strlen(constrain) > 0) {
    msg += ":" + std::string(constrain) + " ";
  }
  msg += " " + std::to_string(value);
  if(!exec(msg))
      return false;

    m_Amplitude = value;
    return true;
}


bool KST33500::enableOutput()
{
    return output(true);
}

bool KST33500::disableOutput()
{
    return output(false);
}

bool KST33500::output(bool on) {
  std::string msg = "OUTPut";
  if (on) {
    msg += " ON";
  } else {
    msg += " OFF";
  }
  if(!exec(msg))
      return false;

    m_OutputEnabled = on;
    return true;
}

bool KST33500::phase(std::string &value) {
  std::string msg = "PHASe " + value;
  return exec(msg);
}

KST33500::FunctionType KST33500::getOutputFunction() const
{
    return m_CurrentFunction;
}

bool KST33500::isOutputEnabled() const
{
    return m_OutputEnabled;
}

double KST33500::getCurrentAmplitude() const
{
    return m_Amplitude;
}

