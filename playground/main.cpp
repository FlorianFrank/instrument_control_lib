#include <iostream>
#include "Device.h"
#include "devices/KEI2600.h"
#include "devices/KST33500.h"
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
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 0.42, false);
    smu->changeSendMode(Device::SEND_METHOD::BUFFER_ENABLED);

    for (int i = 0; i < 10; ++i) {
        smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, i / 100.0, false);
        smu->measure(SMU::VOLTAGE, SMU::CHANNEL_A, nullptr, false);
    }

    smu->executeBufferedScript(false);

    smu->changeSendMode(Device::DIRECT_SEND);

    std::cout << smu->getBufferedScript() << std::endl;

    int bufferSize;
    smu->getBufferSize(smu->CHANNEL_A_BUFFER, &bufferSize, false);
    double buffer[bufferSize];

    smu->readBuffer(smu->CHANNEL_A_BUFFER, buffer, false);

    for (const auto &item: buffer) {
        std::cout << item << std::endl;
    }

    smu->Disconnect();
}

void kstTest() {
    auto *kst = new KST33500("132.231.14.174", 2000);
    kst->Connect();
    kst->setFunction(KST33500::FUNCTION_TYPE::DC_VOLTAGE);
    kst->setOffset(0.5);
    kst->turnOn();
    kst->Disconnect();
}


int main() {
    kstTest();
    return 0;
}
