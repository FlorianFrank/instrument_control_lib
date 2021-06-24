//
// Created by liuwuhao on 17.06.21.
//
#include <iostream>
#include "kst3000.h"
#include <unistd.h>

/**
 * @class KST3000
 * @brief Mixed Single Oscilloscope(Oscillator)
 * Commend operations of a single generator are defined.
 * KST3000 is the model of the device(keysight 3000 series).
 */


/**
 * @brief Constructor
 * @param ip: IP address of kst3000
 * @note Get oscillator's ip: Press "Utility" key; Press I/O;
 * */
KST3000::KST3000(char *ip) : Device(ip) {
    this->name = "Mixed Single Oscilloscope";
}

/**
 * @brief Display a line of text: Connected Successfully. Returning...
 *        The message will disappear after 2 seconds.
 *        Can be used to make sure the connection is working.
 * */
int KST3000::display_connection() {
    exec("DISP:TEXT 'Connected Successfully. Returning...'");
    sleep(2);
    exec("DISP ON");
    return exec("DISPlay:TEXT:CLEar");
}

/**
 * @brief Start to run. Equivalent to press the Run/Stop button when the device is not running.
 * */
int KST3000::run() {
    return exec("RUN");
}

/**
 * @brief Stop to run. Equivalent to press the Run/Stop button when the device is not stopping.
 * */
int KST3000::stop() {
    return exec("STOP");
}

/**
 * @brief Equivalent to press the Single button.
 * */
int KST3000::single() {
    return exec("SINGLE");
}

/**
 * @brief Equivalent to press the "Auto Scale" button.
 * */
int KST3000::autoscale() {
    return exec("AUToscale");
}

/**
 * @brief Set trigger slope
 * @param slope: {POS | NEG | EITH | ALT}
 * */
int KST3000::set_trigger_edge(char *slope) {
    string s_slope(slope);
    return exec("TRIGger:SLOPe " + s_slope);
}

/**
 * @brief Set trigger  channel
 * @param channel: {1(default) | 2 | 3 | 4}
 * */
int KST3000::set_trigger_source(int channel) {
    return exec("TRIGger:SOURce CHAN" + to_string(channel));
}

/**
 * @brief Set timebase(horizontal) range; Equivalent to adjust the "Horizontal" knob.
 * @param range: time range to set, unit: second
 * */
int KST3000::set_time_range(double range) {
    return exec("TIMebase:RANGe " + to_string(range));
}

/**
 * @brief Set timebase(horizontal) delay; Equivalent to adjust the delay knob in Horizontal controls.
 * @param delay: time delay, unit seconds
 * */
int KST3000::set_time_delay(double delay) {
    return exec("TIMebase:DELay " + to_string(delay));
}

/**
 * @brief Equivalent to adjust the knob above channel numbers.
 * @param scale: units(voltage) per division(grid)
 * @param channel:  target channel: default 1
 * */
int KST3000::set_channel_scale(double scale, int channel) {
    // set the vertical scale
    string command = "CHANnel" + to_string(channel) + ":SCALe " + to_string(scale);
    return exec(command);
}

/**
 * @brief Similar to set_channel_scale, but set the whole voltage range.
 * @param range: voltage range(vertical)
 * @param target: channel: default 1
 * @param is_v: voltage unit:
 *              - true: use V as unit, default
 *              - false: use mV as unit
 * */
int KST3000::set_channel_range(double range, int channel, bool is_v) {
    // set the vertical range
    string command = "CHANnel" + to_string(channel) + ":RANGe " + to_string(range);
    if (!is_v) {
        command += " mV";
    }
    return exec(command);
}

/**
 * @brief Set vertical offset. Equivalent to adjust the knob below channel number button
 * @param offset:  vertical offset, unit: V
 * @param channel: channel number, default 1
 * */
int KST3000::set_channel_offset(double offset, int channel) {
   // set the vertical offset
   string command = "CHANnel" + to_string(channel) + ":offset " + to_string(offset);
   return exec(command);
}

/**
 * @brief Show or hide channel display. Equivalent to press channel number button.
 * @param on: 1 or 0. 1: show channel single. 0: hide channel single.
 * @param channel: channel number, default 1
 * */
int KST3000::set_channel_display(int on, int channel) {
    // turns the channel on(1) or off(0)
    string command = "CHANnel" + to_string(channel) + ":DISPlay " + to_string(on);
    return exec(command);
}

/**
 * @brief Query the number of waveform points to be transferred
 * @return the number of waveform points to be transferred
 * */
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
    cout << sizeof(buffer) << endl;
    cout << 1123 << endl;
    for (int i = 0; i < num; i++) {
        cout << hex << (int)buffer[i];
    }
}

/**
 * @brief capture data
 * */
int KST3000::digitize() {
    return exec("DIGitize");
}

/**
 * @brief get system setup
 * */
int KST3000::get_system_setup(char *buffer) {
    return exec("SYSTem:SETup?", buffer);
}