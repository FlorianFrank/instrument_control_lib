//
// Created by liuwuhao on 20.05.21.
//
#include "spd1305.h"

#include <iostream>

int main() {
    SPD1305 s = SPD1305("132.231.14.162" );
    s.connect();
    // setCurrent
    s.setCurrent(2.458);

    // getCurrent
    double c = s.getCurrent();
    cout << c;

    // exec without result
    // s.exec("CH1:CURRent 3.14159");

    // exec with a result(buffer)
    // char buffer[1024] = {0};
    // s.exec("CH1:CURRent?", buffer);
    // cout << buffer;
    return 0;
}

