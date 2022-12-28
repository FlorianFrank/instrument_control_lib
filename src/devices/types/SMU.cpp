#include "devices/types/SMU.h"

#include <utility> // std::move

SMU::SMU(std::string ipAddress, int timeoutInMs): Device(std::move(ipAddress), timeoutInMs)
{

}

SMU::SMU(std::string ipAddress, int timeoutInMs, PIL::Logging *logger): Device(std::move(ipAddress), timeoutInMs, logger)
{
}

