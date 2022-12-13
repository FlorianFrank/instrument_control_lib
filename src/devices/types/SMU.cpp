//
// Created by florianfrank on 19.07.22.
//
#include "devices/types/SMU.h"

SMU::SMU(const char *ip, int timeoutInMs, SEND_METHOD mode): Device(ip, timeoutInMs, mode)
{

}

SMU::SMU(const char *ip, int timeoutInMs, PIL::Logging *logger, SEND_METHOD mode): Device(ip, timeoutInMs, logger, mode)
{
}

