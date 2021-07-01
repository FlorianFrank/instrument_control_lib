//
// Created by liuwuhao on 20.05.21.
//
#include <iostream>
#include <unistd.h>
#include <sstream>
#include "Src/kst33500.h"
#include "Src/spd1305.h"
#include "Src/kst3000.h"

using namespace std;

string cap_commands = "ACQuire:TYPE NORMal\n"
                      "ACQuire:COMPlete 100\n"
                       "ACQuire:COUNt 8\n"
//                       "DIGitize CHANnel1\n";
                       "WAVeform:SOURce CHANnel1\n"
                       "WAVeform:FORMat BYTE\n"
                       "WAVeform:POINts 1000";

int run_kst3000() {
    KST3000 k = KST3000("132.231.14.172");
    k.connect();
//    k.set_time_range(0.05);
//    k.set_channel_scale(3.3);
//    k.set_channel_range(40);
//    k.set_channel_offset(-2);
//    k.set_channel_display(1);
//    k.set_time_delay(0.003);
//    k.set_trigger_edge("NEG");

//    k.set_trigger_source();
//    k.run();
//    char buffer[2048];
//    k.get_system_setup(buffer);
//    cout << buffer;
    k.exec_commands(cap_commands);
//    k.single();
//    k.get_waveform_preamble();
//    int num = k.get_waveform_points();
//    cout << num;
//    k.get_waveform_data();
//    k.cli();
    k.save_waveform_data("/tmp/buffer");
    k.cli();
//    cout << k.get_waveform_points();
//    char buffer[1024] = {0};
//    k.exec("RUN");
//    cout << buffer;
}

int run_kst33500() {
     KST33500 k = KST33500("132.231.14.152");
     k.connect();
     k.what_am_i();
     // k.display_connection();
//     k.cli();
//     k.function("SIN");
//     k.frequency("+1.0E+05");
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
    s.set_current(1.8);

    s.turnon();
    sleep(3);
    s.turnoff();

    double c = s.get_current();
    cout << c;

    s.exec("CH1:CURRent 1.14159");

    char buffer[1024] = {0};
    s.exec("CH1:CURRent?", buffer);
    cout << buffer;
}

int main() {
//    run_kst33500();
//    run_spd();
    run_kst3000();
    return 0;
}




