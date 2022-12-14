#include <iostream>
#include "Device.h"
#include "devices/KEI2600.h"
#include "ctlib/Logging.hpp"
#include "devices/types/SMU.h"

int sweep() {
    std::string ip = "132.231.14.168";
    auto *smu = new KEI2600(ip.c_str(), 0, nullptr, Device::BUFFER_ENABLED);
    smu->Connect();

    double value = 0.0;

    smu->turnOn(SMU::SMU_CHANNEL::CHANNEL_A, false);
    smu->setLimit(SMU::VOLTAGE, SMU::SMU_CHANNEL::CHANNEL_A, 3, false);

    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 1, false);
    std::cout << smu->delay(1) << std::endl;
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 2, false);
    smu->delay(1);
    smu->turnOff(SMU::SMU_CHANNEL::CHANNEL_A, false);

    // smu->measure(SMU::VOLTAGE, SMU::CHANNEL_A, &value, false);

    std::cout << smu->performLinearVoltageSweep(SMU::SMU_CHANNEL::CHANNEL_A, 0.0, 0.55, 18, 0.01, false) << std::endl;

    std::cout << "Buffered Script: " + smu->getBufferedScript() << std::endl;

    smu->changeSendMode(SMU::DIREKT_SEND);
    std::cout << smu->GetDeviceIdentifier() << std::endl;
    smu->changeSendMode(SMU::BUFFER_ENABLED);

    smu->executeBufferedScript(false);

    smu->Disconnect();

    return 0;
}

int main(int argc, char* argv[]) {
    return sweep();
}
