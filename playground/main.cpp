#include <iostream>
#include "Device.h"
#include "devices/KEI2600.h"
#include "ctlib/Logging.hpp"
#include "devices/types/SMU.h"

int sweep() {
    std::string ip = "132.231.14.168";
    auto *smu = new KEI2600(ip.c_str(), 0, nullptr, Device::BUFFER_ENABLED);
    // smu->Connect();

    smu->setLevel(SMU::CURRENT, SMU::CHANNEL_A, 1, false);
    smu->setLevel(SMU::CURRENT, SMU::CHANNEL_A, 2, false);
    smu->setLevel(SMU::CURRENT, SMU::CHANNEL_A, 3, false);
    smu->setLevel(SMU::CURRENT, SMU::CHANNEL_A, 4, false);
    std::cout << smu->delay(10) << std::endl;
    std::cout << smu->performLinearVoltageSweep(SMU::SMU_CHANNEL::CHANNEL_A, 0.0, 0.5, 18, 0.01, false) << std::endl;

    std::cout << "Buffered Script: " + smu->getBufferedScript() << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {
    return sweep();
}
