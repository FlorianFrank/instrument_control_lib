#include <iostream>
#include "Device.h"
#include "devices/KEI2600.h"
#include "ctlib/Logging.hpp"
#include <thread>

void check_error_code(PIL_ERROR_CODE code) {
    if (code != PIL_ERROR_CODE::PIL_NO_ERROR) {
        std::cout << "The last function returned an error code indicating something went wrong." << std::endl;
        throw *new std::runtime_error("");
    }
}

int main() {
    std::string ip = "132.231.14.168";
    PIL::Logging logger(PIL::INFO, nullptr);
    auto *smu = new KEI2600(ip, 2000, &logger, Device::SEND_METHOD::BUFFER_ENABLED);
    PIL_ERROR_CODE errorCode;

    errorCode = smu->Connect();
    check_error_code(errorCode);

    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 0.42, false);

    smu->changeSendMode(Device::SEND_METHOD::BUFFER_ENABLED);

    for (int i = 0; i < 10; ++i) {
        smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, i / 10.0, false);
        smu->measure(SMU::VOLTAGE, SMU::CHANNEL_A, nullptr, false);
        smu->delay(0.1);
    }

    errorCode = smu->executeBufferedScript(false);
    check_error_code(errorCode);

    smu->changeSendMode(Device::DIRECT_SEND);

    int bufferSize;
    smu->getBufferSize(smu->CHANNEL_A_BUFFER, &bufferSize, false);
    std::vector<double> buffer;

    smu->readBuffer("A_M_BUFFER", &buffer, false);

    std::cout << "Measurement returned the following values: " << std::endl;
    for (double value : buffer) {
        std::cout << value << ", ";
    }

    smu->Disconnect();

    return 0;
}
