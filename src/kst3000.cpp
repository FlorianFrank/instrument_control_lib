//
// Created by liuwuhao on 17.06.21.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
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
 * @param slope: {POS | NEG | EITH | ALT} (Rising | Falling | Either | Alternating)
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

vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

/**
 * @brief Query the preamble of waveform data
 * @return a array containing preamble
 * [Waveform format, Acquire type, Waveform points desired, Waveform average count,
 *  Waveform X increment, Waveform X origin, Waveform X reference,
 *  Waveform Y increment, Waveform Y origin, Waveform Y reference]
 * */
char* KST3000::get_waveform_preamble() {
    string command = "WAVeform:PREamble?";
    char buffer[1024] = {0};
    exec(command, buffer);
    // vector<string> substrs = split(buffer, ",");
    return buffer;
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

/**
 * @brief set number of waveform points
 * */
int KST3000::set_waveform_points(int num_points) {
    string command = "WAVeform:POINts" + to_string(num_points);
    return exec(command);
}

/**
 * @brief set format of waveform data(default "BYTE")
 * */
int KST3000::set_waveform_format(string format) {
    string command = "WAVeform:FORMat" + format;
    return exec(command);
}

/**
 * @brief write to file
 * */
int write_to_file(string data, string file_path) {
    string buffer(data);
    ofstream file;
    file.open(file_path);
    file  << buffer;
    file.close();
    return 0;
}

/**
 * @brief get the sampled data points
 * @details READ_WAVE_DATA - The wave data consists of two parts: the header,
   and the actual waveform data followed by a new line (NL) character.\n

   The query data has the following format:\n
        <header><waveform_data><NL>\n
   Where:\n
   <header> = #800001000 (This is an example header)\n
   The "#8" may be stripped off of the header and the remaining
   numbers are the size, in bytes, of the waveform data block.
   The size can vary depending on the number of points acquired for the waveform.(In the example, 1000 points)
   You can then read that number of bytes from the oscilloscope and the terminating NL character.
 * */
int KST3000::get_waveform_data(char *data) {
    string command = "WAVeform:DATA?";
    int num = get_waveform_points();
    char buffer[num + 10]; // 10 is the length of <header>
    memset(buffer, 0, num);
    exec(command, buffer, true, num);
    memcpy(data, buffer + 10, num);
    return 0;
}

/**
 * @brief save waveform data to the target file
 * @details The file can be plotted, for example using python.
 * @code{.py}
 * import pandas as pd
 * import matplotlib.pyplot as plt
 * data = pd.read_csv("buffer")
 * plt.xlabel('time(ms)')
 * plt.ylabel('voltage(V)')
 * plt.plot(data['time(ms)'], data['voltage(V)'])
 * @endcode
 * */
int KST3000::save_waveform_data(string file_path) {
    char *preamble = get_waveform_preamble();
    vector<string> v_preamble = split(preamble, ",");
    int points = stoi(v_preamble[2]);
    double x_increment = stod(v_preamble[4]);
    double x_origin = stod(v_preamble[5]);
    double x_reference = stod(v_preamble[6]);
    double y_increment = stod(v_preamble[7]);
    double y_origin = stod(v_preamble[8]);
    double y_reference = stod(v_preamble[9]);

    char data[points];
    get_waveform_data(data);

    stringstream stream;
    stream << "time(ms)" << "," << "voltage(V)" << endl;
    for (int i = 0; i < points; i++) {
        double time = ((i - x_reference) * x_increment) + x_origin;
        int voltage_data = (int) (unsigned char) data[i];
//        if (voltage_data == 0) {
//            continue;
//        }
        double voltage = ((voltage_data - y_reference) * y_increment) + y_origin;
        stream << time*1000 << "," << voltage << endl;
    }

    write_to_file(stream.str(), file_path);
    return 0;
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
