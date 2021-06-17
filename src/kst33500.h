//
// Created by liuwuhao on 27.05.21.
//

#ifndef CE_DEVICE_KST33500_H
#define CE_DEVICE_KST33500_H

#include "device.h"
using namespace std;

class KST33500 : public Device {
public:
    KST33500(char *ip);
    int display(string text);
    int display_connection();
    int function(string fun);
    int frequency(string value);
    int voltage(string value, string constrain = "");
    int output(bool on);
    int phase(string value);
};

#endif //CE_DEVICE_KST33500_H
