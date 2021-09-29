//
// Created by liuwuhao on 27.05.21.
//

#include "kst33500.h"
#include <unistd.h>

using namespace std;

KST33500::KST33500(char *ip) : Device(ip) {
  this->name = "Keysight 33500B Waveform Generator";
}

int KST33500::display(string text) {
  string msg = "DISP:TEXT '" + text + "'";
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

int KST33500::function(string fun) {
  string msg = "FUNCtion " + fun;
  exec(msg);
  return 0;
}

int KST33500::frequency(double value) {
  string msg = "FREQuency " + to_string(value);
  exec(msg);
  return 0;
}

int KST33500::voltage(double value, string constrain) {
  string msg = "VOLTage";
  if (constrain != "") {
    msg += ":" + constrain + " ";
  }
  msg += " " + to_string(value);
  exec(msg);
  return 0;
}

/**
 * argument unit: V
 * attention: offset would change voltage by double of your argument
 * */
int KST33500::offset(double value) {
  string msg = "VOLTage:OFFSet ";
  msg += to_string(value);
  return exec(msg);
}

/**
 * argument unit: ms
 * */
int KST33500::set_pulse_width(double value) {
  string msg = "FUNCtion:PULSe:WIDTh ";
  msg += to_string(value) + " ms";
  return exec(msg);
}

int KST33500::output(bool on) {
  string msg = "OUTPut";
  if (on) {
    msg += " ON";
  } else {
    msg += " OFF";
  }
  exec(msg);
  return 0;
}

int KST33500::phase(string value) {
  string msg = "PHASe " + value;
  exec(msg);
  return 0;
}