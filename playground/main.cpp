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
    auto *smu = new KEI2600(ip, 0, &logger, Device::SEND_METHOD::BUFFER_ENABLED);

    smu->Connect();

    for (int i = 0; i < 10; ++i) {
        smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, (0.42 + (i / 100.0)), false);
        smu->delay(i / 5.0);
    }

    std::cout << smu->getBufferedScript();

    std::cout << smu->executeBufferedScript(false) << std::endl;

    smu->Disconnect();
}

void sweepTest() {
    std::string ip = "132.231.14.168";
    PIL::Logging logger(PIL::INFO, nullptr);
    auto *smu = new KEI2600(ip, 0, &logger, Device::SEND_METHOD::DIRECT_SEND);

    smu->Connect();
    smu->changeSendMode(Device::SEND_METHOD::BUFFER_ENABLED);
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 0.3, false);
    smu->performLinearVoltageSweep(SMU::CHANNEL_A, 0, 0.42, 18, 0.001, false);
    smu->Disconnect();
}

void measurementBufferingTest() {
    std::string ip = "132.231.14.168";
    PIL::Logging logger(PIL::INFO, nullptr);
    auto *smu = new KEI2600(ip, 0, &logger, Device::SEND_METHOD::DIRECT_SEND);

    smu->Connect();
    smu->changeSendMode(Device::SEND_METHOD::BUFFER_ENABLED);

    for (int i = 0; i < 10; ++i) {
        smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, (0.42 + (i / 100.0)), false);
        smu->measure(SMU::CURRENT, SMU::CHANNEL_A, nullptr, false);
    }

    smu->executeBufferedScript(false);

    smu->changeSendMode(Device::DIRECT_SEND);

    std::cout << smu->getBufferedScript() << std::endl;

    int bufferSize;
    smu->getBufferSize("A_M_BUFFER", &bufferSize, false);

    std::cout << "Buffer size: " << bufferSize << std::endl;
    double buffer[bufferSize];

    smu->readBuffer("A_M_BUFFER", buffer, false);

    for (const auto &item: buffer) {
        std::cout << item << std::endl;
    }

    smu->Disconnect();
}


int main() {
    measurementBufferingTest();
    return 0;
}
