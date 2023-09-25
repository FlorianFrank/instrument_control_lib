#include <unistd.h>
#include <iostream>
#include "Device.h"
#include "devices/KEI2600.h"
#include "devices/KST3000.h"
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

    std::cout << smu->measurePy(SMU::VOLTAGE, SMU::CHANNEL_A, false);

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

    for (int i = 0; i < 20; ++i) {
        smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, i / 100.0, false);
        smu->measure(SMU::VOLTAGE, SMU::CHANNEL_A, nullptr, false);
    }

    std::cout << smu->getBufferedScript() << std::endl;

    smu->changeSendMode(Device::DIRECT_SEND);
    smu->performLinearVoltageSweep(SMU::CHANNEL_A, 0, 0.11, 18, 0.1, false);
    smu->delay(10);
    smu->changeSendMode(Device::BUFFER_ENABLED);

    smu->executeBufferedScript(false);

    smu->changeSendMode(Device::DIRECT_SEND);

    std::cout << smu->getBufferedScript() << std::endl;

    int bufferSize;
    smu->getBufferSize(smu->CHANNEL_A_BUFFER, &bufferSize, false);
    std::cout << "Buffer size: " << bufferSize << std::endl;
    std::vector<double> buffer;

    smu->readBuffer(smu->CHANNEL_A_BUFFER, &buffer, false);

    for (double val: buffer) {
        std::cout << val << std::endl;
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

void sleep(int secs) {
    unsigned int microsecond = 1000000;
    usleep(secs * microsecond);
}

void oscTest() {
    const char *ip = "132.231.14.181";
    PIL::Logging logger(PIL::INFO, nullptr);
    KST3000 o = KST3000(ip, 30000, &logger);
    o.Connect();

    o.setDisplayMode(KST3000::DISPLAY_MODES::MAIN);
    o.setTimeRange(1);

    o.setChannelRange(KST3000::CHANNEL_1, 1.6, Oscilloscope::VOLT);
    o.setChannelRange(KST3000::CHANNEL_2, 1.6, Oscilloscope::VOLT);
    o.setWaveformPoints(1000);

    for (int i = 1; i <= 1; i++) {
        double voltage = i / 10.0;
        std::string file_prefix =
                "/home/alexander/Work/Data/memristor/test_data_1";

        std::string data1 = file_prefix + "_1";
        std::string data2 = file_prefix + "_2";

        o.digitize(KST3000::CHANNEL_1);
        o.setWaveformSource(KST3000::OSC_CHANNEL::CHANNEL_1);
        o.saveWaveformData(data1);
        o.digitize(KST3000::CHANNEL_2);
        o.setWaveformSource(KST3000::OSC_CHANNEL::CHANNEL_2);
        o.saveWaveformData(data2);
        sleep(1);
    }
}

DWORD WINAPI ThreadFunction(void *threadArgs) {
    return NULL;
}


int main() {
    std::cout << ThreadFunction << std::endl;
    // oscTest();
    return 0;
}

