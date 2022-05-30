//
// Created by liuwuhao on 20.05.21.
//
#include <iostream>
#include <cstring>
#include "spd1305.h"

SPD1305::SPD1305(const char *ip) : Device(ip) {
  this->m_IPAddr = ip;
  this->m_DeviceName = "DC Power Supply";
}

int SPD1305::set_current(double current, int channel) {
  std::string msg = "CH" + std::to_string(channel) + ":CURRent " + std::to_string(current);
    Exec(msg);
  return 0;
}

double SPD1305::get_current(int channel) {
  std::string msg = "CH" + std::to_string(channel) + ":CURRent?";
  // TODO: 20 ?
  char result[20] = {0};
    Exec(msg, result);
  return atof(result);
}

int SPD1305::turnon(int channel) {
  std::string msg = "OUTPut CH" + std::to_string(channel) + ",ON";
    Exec(msg, nullptr, false);
  return 0;
}

int SPD1305::turnoff(int channel) {
  std::string str = "OUTPut CH" + std::to_string(channel) + ",OFF";
  std::cout << str;
  char command[str.length() + 1];
  strcpy(command, str.c_str());
    Exec(command, nullptr, false);
  return 0;
}
