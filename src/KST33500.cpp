//
// Created by liuwuhao on 27.05.21.
//

#include "KST33500.h"
#include <unistd.h>

using namespace std;

KST33500::KST33500(const char *ip) : Device(ip) {
  this->m_DeviceName = "Keysight 33500B Waveform Generator";
}

bool KST33500::display(std::string &text) {
  string msg = "DISP:TEXT '" + text + "'";
    Exec(msg);
  return 0;
}

bool KST33500::display_connection() {
    Exec("DISP:TEXT 'Connected Successfully. Returning...'");
  sleep(2);
    Exec("DISP ON");
    Exec("DISPlay:TEXT:CLEar");
  return 0;
}

bool KST33500::function(string& fun) {
  string msg = "FUNCtion " + fun;
    Exec(msg);
  return 0;
}

bool KST33500::frequency(double value) {
  string msg = "FREQuency " + to_string(value);
    Exec(msg);
  return 0;
}

bool KST33500::voltage(double value, const char *constrain)
{
  string msg = "VOLTage";
  if (constrain != "") {
  //  msg += ":" + constrain + " "; TODO
  }
  msg += " " + to_string(value);
    Exec(msg);
  return 0;
}

/**
 * argument unit: V
 * attention: offset would change voltage by double of your argument
 * */
int KST33500::offset(double value) {
  string msg = "VOLTage:OFFSet ";
  msg += to_string(value);
  return Exec(msg);
}

/**
 * argument unit: ms
 * */
int KST33500::set_pulse_width(double value) {
  string msg = "FUNCtion:PULSe:WIDTh ";
  msg += to_string(value) + " ms";
  return Exec(msg);
}

bool KST33500::output(bool on) {
  string msg = "OUTPut";
  if (on) {
    msg += " ON";
  } else {
    msg += " OFF";
  }
    Exec(msg);
  return 0;
}

int KST33500::phase(string value) {
  string msg = "PHASe " + value;
    Exec(msg);
  return 0;
}