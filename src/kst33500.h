//
// Created by liuwuhao on 27.05.21.
//

#ifndef CE_DEVICE_KST33500_H
#define CE_DEVICE_KST33500_H

#include "device.h"


class KST33500 : public Device {

public:
    explicit KST33500(const char *ip);

    int display(std::string &text);

    int display_connection();

    int function(std::string fun);

    int frequency(double value);

    int voltage(double value, std::string constrain = "");

    int output(bool on);

    int phase(std::string &value);
};

#endif //CE_DEVICE_KST33500_H
