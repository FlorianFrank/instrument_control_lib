#include "devices/types/DCPowerSupply.h"

DCPowerSupply::DCPowerSupply(const char *ip, int timeoutInMs, PIL::Logging *logger)
        : Device(ip, timeoutInMs, logger, Device::DIRECT_SEND) {

}
