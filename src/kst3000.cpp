//
// Created by liuwuhao on 17.06.21.
//
#include <iostream>
#include "kst3000.h"
#include <unistd.h>

KST3000::KST3000(char *ip) : Device(ip) {
    this->name = "Mixed Single Oscilloscope";
}

int KST3000::display_connection() {
    exec("DISP:TEXT 'Connected Successfully. Returning...'");
    sleep(2);
    exec("DISP ON");
    return exec("DISPlay:TEXT:CLEar");
}

int KST3000::run() {
    return exec("RUN");
}

int KST3000::stop() {
    return exec("STOP");
}

int KST3000::single() {
    return exec("SINGLE");
}

int KST3000::autoscale() {
    return exec("AUToscale");
}

int KST3000::set_time_range(double range) {
    return exec("TIMebase:RANGe " + to_string(range));
}

int KST3000::set_channel_scale(double scale, int channel) {
    // set the vertical scale
    string command = "CHANnel" + to_string(channel) + ":SCALe " + to_string(scale);
    return exec(command);
}

int KST3000::set_channel_range(double range, int channel, bool is_v) {
    // set the vertical range
    string command = "CHANnel" + to_string(channel) + ":RANGe " + to_string(range);
    if (!is_v) {
        command += " mV";
    }
    return exec(command);
}

int KST3000::set_channel_offset(double range, int channel) {
   // set the vertical offset
   string command = "CHANnel" + to_string(channel) + ":offset " + to_string(range);
   return exec(command);
}

int KST3000::set_channel_display(int on, int channel) {
    // turns the channel on(1) or off(0)
    string command = "CHANnel" + to_string(channel) + ":DISPlay " + to_string(on);
    return exec(command);
}

int KST3000::get_waveform_points() {
    string command = "WAVeform:POINts?";
    char buffer[1024] = {0};
    exec(command, buffer);
    return stoi(buffer);
}

char* KST3000::get_waveform_data() {
    string command = "WAVeform:DATA?";
    int num = get_waveform_points();
    char buffer[num];
    memset(buffer, 0, num);
    exec(command, buffer);
    for (int i = 0; i < num; i++) {
        cout << hex << (int)buffer[i];
    }
}
