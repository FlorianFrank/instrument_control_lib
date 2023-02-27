#include <iostream>
#include "Device.h"
#include "devices/KEI2600.h"
#include "ctlib/Logging.hpp"
#include <thread>

int main() {
    std::string ip = "132.231.14.168";
    PIL::Logging logger(PIL::INFO, nullptr);
    auto *smu = new KEI2600(ip, 0, &logger, Device::SEND_METHOD::DIRECT_SEND);

    smu->Connect();
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 0.42, false);
    smu->changeSendMode(Device::SEND_METHOD::BUFFER_ENABLED);

    for (int i = 0; i < 10; ++i) {
        smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, i / 100.0, false);
        smu->measure(SMU::VOLTAGE, SMU::CHANNEL_A, nullptr, false);
        smu->delay(0.001);
    }

    smu->executeBufferedScript(false);

    smu->changeSendMode(Device::DIRECT_SEND);
    std::cout << smu->getBufferedScript() << std::endl;

    int bufferSize;
    smu->getBufferSize("A_M_BUFFER", &bufferSize, false);
    double buffer[bufferSize];

    smu->readBuffer("A_M_BUFFER", buffer, false);

    for (double value : buffer) {
        std::cout << value << std::endl;
    }

    smu->Disconnect();

    return 0;
}
