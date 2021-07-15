//
// Created by liuwuhao on 15.07.21.
//
#include <iostream>
#include <filesystem>
#include <unistd.h>

#include "kst3000.h"
#include "kst33500.h"

using namespace std;


int main() {
    char *wave_generator_ip = "132.231.14.246";
    KST33500 wg = KST33500(wave_generator_ip);
    wg.connect();
    wg.frequency(100);

    char *ip = "132.231.14.247";
    KST3000 k = KST3000(ip);
    k.connect();
    k.set_timebase_mode("MAIN");
    k.set_waveform_points(1000);
    k.set_channel_range(1.6, 1);
    k.set_channel_range(1.6, 2);

    for (int i = 1; i <= 12; i++) {
        double voltage = i / 10.0;
        cout << voltage << endl;
        wg.voltage(voltage);
        string file_prefix = "/tmp/data/1_" + to_string(i);
        cout << file_prefix << endl;

        k.digitize();
        k.set_waveform_source(1);
        k.save_waveform_data(file_prefix + "_1");
        k.set_waveform_source(2);
        k.save_waveform_data(file_prefix + "_2");

        unsigned int microsecond = 1000000;
        usleep(1 * microsecond);//sleeps for 3 second
    }

//    k.digitize();
//    k.set_waveform_source(1);
//    k.save_waveform_data(file_prefix + "_1");
//    k.set_waveform_source(2);
//    k.save_waveform_data(file_prefix + "_2");

    return 0;
}

