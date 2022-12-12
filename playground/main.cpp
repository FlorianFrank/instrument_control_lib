#include <iostream>
#include "Device.h"
#include "devices/KEI2600.h"
#include "unistd.h"
#include "ctlib/Logging.hpp"

int sweep() {
    std::string ip = "132.231.14.168";
    auto *smu = new KEI2600(ip.c_str(), 0, nullptr);
    smu->Connect();

    std::string id = smu->GetDeviceIdentifier();
    std::cout << "Connected to: " + id << std::endl;

    std::cout << smu->linearVoltageSweep() << std::endl;

    return 0;
}

int main(int argc, char* argv[]) {
    return sweep();
}
