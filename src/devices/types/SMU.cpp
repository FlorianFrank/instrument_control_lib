#include "devices/types/SMU.h"

#include <utility> // std::move

SMU::SMU(std::string ipAddress, int timeoutInMs, SEND_METHOD mode)
        : Device(std::move(ipAddress), timeoutInMs, mode) {

}

SMU::SMU(std::string ipAddress, int timeoutInMs, PIL::Logging *logger, SEND_METHOD mode)
        : Device(std::move(ipAddress), timeoutInMs, logger, mode) {
}
