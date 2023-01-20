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

PIL_ERROR_CODE SPD1305::setCurrent(DC_CHANNEL channel, double current)
{
  SubArg surrentArg("CURRent", ":");
  ExecArgs args;
           args.AddArgument("CH", getStrFromDCChannelEnum(channel))
                .AddArgument(surrentArg, current);

  return Exec("OUTPut", &args);
}

PIL_ERROR_CODE SPD1305::getCurrent(DC_CHANNEL channel, double *current) {
  std::string msg = "CH" + getStrFromDCChannelEnum(channel) + ":CURRent?";
  std::string result;
  auto execRet = Exec(msg, nullptr, &result, false);
  if(execRet != PIL_NO_ERROR)
      return execRet;
  *current = atof(result.c_str());
  return PIL_NO_ERROR;
}

PIL_ERROR_CODE SPD1305::turnOn(DC_CHANNEL channel) {
    SubArg surrentArg("CURRent", ":");
    ExecArgs args;
    args.AddArgument("CH", getStrFromDCChannelEnum(channel))
            .AddArgument("", "ON", ",");

    return Exec("OUTPut", &args);
}

PIL_ERROR_CODE SPD1305::turnOff(DC_CHANNEL channel) {
    SubArg surrentArg("CURRent", ":");
    ExecArgs args;
    args.AddArgument("CH", getStrFromDCChannelEnum(channel))
            .AddArgument("", "OFF", ",");

    return Exec("OUTPut", &args);
}

std::string SPD1305::getStrFromDCChannelEnum(DCPowerSupply::DC_CHANNEL channel)
{
    switch (channel)
    {
        case DCPowerSupply::CHANNEL_1:
            return "1";
        case DCPowerSupply::CHANNEL_2:
            return "2";
        default:
            if(m_EnableExceptions)
                throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "Invalid channel");
            return "";
    }
}
