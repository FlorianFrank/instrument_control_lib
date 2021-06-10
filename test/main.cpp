//
// Created by liuwuhao on 20.05.21.
//
#include <iostream>
#include <unistd.h>
#include "Src/kst33500.h"
#include "Src/spd1305.h"

int run_kst() {
     KST33500 k = KST33500("132.231.14.152");
     k.connect();
     k.display_connection();
//     k.function("SIN");
//     k.frequency("+1.0E+03");
//     k.voltage("+2.0", "HIGH");
//     k.voltage("+0.0", "LOW");
//     k.output(true);
//     sleep(3);
//     k.output(false);
//     k.phase("+90.0");
}

int run_spd() {

    SPD1305 s = SPD1305("132.231.14.162" );
    s.connect();
    s.setCurrent(1.8);

    s.turnon();
    sleep(3);
    s.turnoff();

    double c = s.getCurrent();
    cout << c;

    s.exec("CH1:CURRent 1.14159");

    char buffer[1024] = {0};
    s.exec("CH1:CURRent?", buffer);
    cout << buffer;
}

int main() {
    run_kst();
    // run_spd();
    return 0;
}

