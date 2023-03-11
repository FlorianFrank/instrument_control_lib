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

void useSMU() {
    std::string ip = "132.231.14.168";
    PIL::Logging logger(PIL::INFO, nullptr);
    auto *smu = new KEI2600(ip, 2000, &logger, Device::SEND_METHOD::DIRECT_SEND);
    PIL_ERROR_CODE errorCode;

    errorCode = smu->Connect();
    check_error_code(errorCode);

    errorCode = smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 0.42, false);
    check_error_code(errorCode);

    double voltage;
    for (int i = 0; i < 10; ++i) {
        errorCode = smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, i / 100.0, false);
        check_error_code(errorCode);

        smu->measure(SMU::VOLTAGE, SMU::CHANNEL_A, &voltage, false);
        std::cout << "Measured " << std::to_string(voltage) << "V on CHANNEL_A" << std::endl;

        smu->delay(0.1);
    }

    errorCode = smu->executeBufferedScript(false);
    check_error_code(errorCode);

    smu->Disconnect();
}

int main() {
    try {
        useSMU();
    } catch (std::runtime_error &ignored) {
        std::cout << "Graceful exit" << std::endl;
    }

    return 0;
}
