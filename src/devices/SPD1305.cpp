//
// Created by liuwuhao on 20.05.21.
//
#include <iostream>
#include <cstring>
#include "devices/SPD1305.h"

SPD1305::SPD1305(const char *ip, int timeoutInMS) : DCPowerSupply(ip, timeoutInMS, nullptr)
{
  this->m_DeviceName = "DC Power Supply";
}

SPD1305::SPD1305(const char *ip, PIL::Logging *logger, int timeoutInMs) : DCPowerSupply(ip, timeoutInMs, logger)
{
  this->m_DeviceName = "DC Power Supply";
}

PIL_ERROR_CODE SPD1305::setCurrent(DC_CHANNEL channel, double current) {
  std::string msg = "CH" + getStrFromDCChannelEnum(channel) + ":CURRent " + std::to_string(current);
  return Exec(msg);
}

PIL_ERROR_CODE SPD1305::getCurrent(DC_CHANNEL channel, double *current) {
  std::string msg = "CH" + getStrFromDCChannelEnum(channel) + ":CURRent?";
  // TODO: 20 ?
  char result[20] = {0};
  auto execRet = Exec(msg, result);
  if(execRet != PIL_NO_ERROR)
      return execRet;
  *current = atof(result);
  return PIL_NO_ERROR;
}

PIL_ERROR_CODE SPD1305::turnOn(DC_CHANNEL channel) {
  std::string msg = "OUTPut CH" + getStrFromDCChannelEnum(channel) + ",ON";
    return Exec(msg, nullptr, false);
}

PIL_ERROR_CODE SPD1305::turnOff(DC_CHANNEL channel) {
  std::string str = "OUTPut CH" + getStrFromDCChannelEnum(channel) + ",OFF";
  std::cout << str;
  char command[str.length() + 1];
  strcpy(command, str.c_str());
  return Exec(command, nullptr, false);
}

/*static*/ std::string SPD1305::getStrFromDCChannelEnum(DCPowerSupply::DC_CHANNEL channel)
{
    switch (channel)
    {
        case DCPowerSupply::CHANNEL_1:
            return "1";
        case DCPowerSupply::CHANNEL_2:
            return "2";
        default:
            return "1"; // TODO
    }
}
