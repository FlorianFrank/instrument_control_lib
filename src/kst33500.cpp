//
// Created by liuwuhao on 27.05.21.
//

#include "kst33500.h"
#include <unistd.h>


KST33500::KST33500(const char *ip) : Device(ip) {
  this->m_DeviceName = "Keysight 33500B Waveform Generator";
}

int KST33500::display(std::string &text) {
  std::string msg = "DISP:TEXT '" + text + "'";
  exec(msg);
  return 0;
}

int KST33500::display_connection() {
  exec("DISP:TEXT 'Connected Successfully. Returning...'");
  sleep(2);
  exec("DISP ON");
  exec("DISPlay:TEXT:CLEar");
  return 0;
}

int KST33500::function(std::string &fun) {
  std::string msg = "FUNCtion " + fun;
  exec(msg);
  return 0;
}

int KST33500::frequency(double value) {
  std::string msg = "FREQuency " + std::to_string(value);
  exec(msg);
  return 0;
}

int KST33500::voltage(double value, std::string constrain) {
  std::string msg = "VOLTage";
  if (!constrain.empty()) {
    msg += ":" + constrain + " ";
  }
  msg += " " + std::to_string(value);
  exec(msg);
  return 0;
}

int KST33500::output(bool on) {
  std::string msg = "OUTPut";
  if (on) {
    msg += " ON";
  } else {
    msg += " OFF";
  }
  exec(msg);
  return 0;
}

int KST33500::phase(std::string &value) {
  std::string msg = "PHASe " + value;
  exec(msg);
  return 0;
}