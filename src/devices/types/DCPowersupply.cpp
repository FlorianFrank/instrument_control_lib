//
// Created by florianfrank on 20.07.22.
//

#include "devices/types/DCPowerSupply.h"

DCPowerSupply::DCPowerSupply(const char *ip, int timeoutInMs, PIL::Logging *logger) : Device(ip, timeoutInMs, logger, Device::DIREKT_SEND)
{

}