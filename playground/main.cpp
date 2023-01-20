#include <iostream>
#include "Device.h"
#include "devices/KEI2600.h"
#include "ctlib/Logging.hpp"
#include <thread>

void generic_test() {
    std::string ip = "132.231.14.168";
    PIL::Logging logger(PIL::INFO, nullptr);
    auto *smu = new KEI2600(ip, 0, &logger, Device::SEND_METHOD::DIRECT_SEND);

    smu->Connect();

    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 0.42, false);

    smu->disableBeep(false);

    std::cout << smu->GetDeviceIdentifier() << std::endl;

    smu->Disconnect();
}

void bufferingTest() {
    std::string ip = "132.231.14.168";
    PIL::Logging logger(PIL::INFO, nullptr);
    auto *smu = new KEI2600(ip, 0, &logger, Device::SEND_METHOD::DIRECT_SEND);

    smu->Connect();

    for (int i = 0; i < 10; ++i) {
        smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, (0.42 + (i / 100.0)), false);
        smu->delay(i / 5.0);
    }

    std::cout << smu->executeBufferedScript(false) << std::endl;

    smu->Disconnect();
}


int main() {
    bufferingTest();
    return 0;
}
