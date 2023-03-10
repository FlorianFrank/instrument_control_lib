#include "devices/types/Oscilloscope.h"

Oscilloscope::Oscilloscope(const char *ip, int timeoutInMs, PIL::Logging *logger)
        : Device(ip, timeoutInMs, logger, Device::DIRECT_SEND) {
}
