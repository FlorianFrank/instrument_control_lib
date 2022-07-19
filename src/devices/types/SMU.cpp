//
// Created by florianfrank on 19.07.22.
//
#include "devices/types/SMU.h"

SMU::SMU(const char *ip, int timeoutInMs, PIL::Logging *logger): Device(ip, timeoutInMs, logger)
{
}
