//
// Created by liuwuhao on 27.05.21.
//

#include <iostream>
#include "kst33500.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

KST33500::KST33500(char *ip) : Device(ip) {
    this->ip = ip;
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

int KST33500::frequency(string value) {
    string msg = "FREQuency " + value;
    exec(msg);
    return 0;
}

int KST33500::voltage(string value, string constrain) {
    string msg = "VOLTage";
    if (constrain != "") {
        msg += ":" + constrain + " ";
    }
    msg += " " + value;
    exec(msg);
    return 0;
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