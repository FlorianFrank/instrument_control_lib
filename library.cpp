#include "library.h"

#include <iostream>

Device::Device(char* ip, unsigned short port) : ip(ip), port(port) {
}

int Device::connect() {
    cout << name + "is connecting...\n";
    return 0;
}

int Device::exec(char* message, char* result = nullptr) {
    cout << message;
    return 0;
}
