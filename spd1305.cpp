//
// Created by liuwuhao on 20.05.21.
//
#include <iostream>
#include "spd1305.h"

using namespace std;

SPD1305::SPD1305(char* ip) : Device(ip) {
    this->ip = ip;
}

int SPD1305::setCurrent(double current, int channel) {
    string msg = "CH" + to_string(channel) + ":CURRent " + to_string(current);
    exec(msg);
    return 0;
}

double SPD1305::getCurrent(int channel) {
    string msg = "CH" + to_string(channel) + ":CURRent?";
    // TODO: 20 ?
    char result[20] = {0};
    exec(msg, result);
    return atof(result);
}

int SPD1305::turnon(int channel) {
    string msg = "OUTPut CH" + to_string(channel) + ",ON";
    exec(msg, nullptr, false);
    return 0;
}

int SPD1305::turnoff(int channel) {
    string str = "OUTPut CH" + to_string(channel) + ",OFF";
    cout << str;
    char command[str.length() + 1];
    strcpy(command, str.c_str());
    exec(command, nullptr, false);
    return 0;
}
