//
// Created by liuwuhao on 17.06.21.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include "KST3000.h"
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
 * @note Get oscillator's m_IPAddr: Press "Utility" key; Press I/O;
 * */
// TODO: these two instructor seem not necessary, could coverd by Device
KST3000::KST3000(const char *ip, int timeoutInMS) : Device(ip, timeoutInMS, nullptr) {
    this->m_DeviceName = "Mixed Single Oscilloscope";
}

KST3000::KST3000(const char *ip, int timeoutInMS, PIL::Logging *logger) : Device(ip, timeoutInMS, logger) {
    this->m_DeviceName = "Mixed Single Oscilloscope";
}


/**
 * @brief Display a line of text: Connected Successfully. Returning...
 *        The message will disappear after 2 seconds.
 *        Can be used to make sure the connection is working.
 * */

int KST3000::display_connection() {
    Exec("DISP:TEXT 'Connected Successfully. Returning...'");
    sleep(2);
    Exec("DISP ON");
    return Exec("DISPlay:TEXT:CLEar");
}

/**
 * @brief Start to run. Equivalent to press the Run/Stop button when the device is not running.
 * */
int KST3000::run() {
    return Exec("RUN");
}

/**
 * @brief Stop to run. Equivalent to press the Run/Stop button when the device is not stopping.
 * */
int KST3000::stop() {
    return Exec("STOP");
}

/**
 * @brief Equivalent to press the Single button.
 * */
int KST3000::single() {
    return Exec("SINGLE");
}

/**
 * @brief Equivalent to press the "Auto Scale" button.
 * */
int KST3000::autoscale() {
    return Exec("AUToscale");
}

/**
 * @brief Set trigger slope
 * @param slope: {POS | NEG | EITH | ALT} (Rising | Falling | Either | Alternating)
 * */
int KST3000::set_trigger_edge(char *slope) {
    std::string s_slope(slope);
    return Exec("TRIGger:SLOPe " + s_slope);
}

/**
 * @brief Set trigger  channel
 * @param channel: {1(default) | 2 | 3 | 4}
 * */
int KST3000::set_trigger_source(int channel) {
    return Exec("TRIGger:SOURce CHAN" + std::to_string(channel));
}

/**
 * @brief Set timebase(horizontal) range; Equivalent to adjust the "Horizontal" knob.
 * @param range: time range to set, unit: second
 * */
int KST3000::set_time_range(double range) {
    return Exec("TIMebase:RANGe " + std::to_string(range));
}

/**
 * @brief Set timebase(horizontal) delay; Equivalent to adjust the delay knob in Horizontal controls.
 * @param delay: time delay, unit seconds
 * */
int KST3000::set_time_delay(double delay) {
    return Exec("TIMebase:DELay " + std::to_string(delay));
}

/**
 * @brief Equivalent to adjust the knob above channel numbers.
 * @param scale: units(voltage) per division(grid)
 * @param channel:  target channel: default 1
 * */
int KST3000::set_channel_scale(double scale, int channel) {
    // set the vertical scale
    std::string command = ""
                          "CHANnel" + std::to_string(channel) + ":SCALe " + std::to_string(scale);
    return Exec(command);
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
    std::string command = "CHANnel" + std::to_string(channel) + ":RANGe " + std::to_string(range);
    if (!is_v) {
        command += " mV";
    }
    return Exec(command);
}

/**
 * @brief Set vertical offset. Equivalent to adjust the knob below channel number button
 * @param offset:  vertical offset, unit: V
 * @param channel: channel number, default 1
 * */
int KST3000::set_channel_offset(double offset, int channel) {
    // set the vertical offset
    std::string command = "CHANnel" + std::to_string(channel) + ":offset " + std::to_string(offset);
    return Exec(command);
}

/**
 * @brief Show or hide channel display. Equivalent to press channel number button.
 * @param on: 1 or 0. 1: show channel single. 0: hide channel single.
 * @param channel: channel number, default 1
 * */
int KST3000::set_channel_display(int on, int channel) {
    // turns the channel on(1) or off(0)
    std::string command = "CHANnel" + std::to_string(channel) + ":DISPlay " + std::to_string(on);
    return Exec(command);
}

std::vector<std::string> split(const char *splitStr, std::string &delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();

    std::string stringToSplit = std::string(splitStr);
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = stringToSplit.find(delimiter, pos_start)) != std::string::npos) {
        token = stringToSplit.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(stringToSplit.substr(pos_start));
    return res;
}

/**
 * @brief Query the preamble of waveform data
 * @return a array containing preamble
 * [Waveform format, Acquire type, Waveform points desired, Waveform average count,
 *  Waveform X increment, Waveform X origin, Waveform X reference,
 *  Waveform Y increment, Waveform Y origin, Waveform Y reference]
 * */
int KST3000::get_waveform_preamble(char *preamble) {
    std::string command = "WAVeform:PREamble?";
    return Exec(command, preamble);
}

/**
 * @brief Query the number of waveform points to be transferred
 * @return the number of waveform points to be transferred
 * */
int KST3000::get_waveform_points() {
    std::string command = "WAVeform:POINts?";
    char buffer[1024] = {0};
    Exec(command, buffer);
    return std::stoi(buffer);
}

/**
 * @brief set waveform points mode
 * @param mode:
 *          - NORMal
 *          - MAXimum
 *          - RAW
 * */
int KST3000::set_waveform_points_mode(std::string &mode) {
    std::string command = "WAVeform:POINts:MODE " + mode;
    return Exec(command);
}

/**
 * @brief set number of waveform points
 * */
int KST3000::set_waveform_points(int num_points) {
    std::string command = "WAVeform:POINts " + std::to_string(num_points);
    return Exec(command);
}

/**
 * @brief set format of waveform data(default "BYTE")
 * */
int KST3000::set_waveform_format(const char *format) {
    std::string command = "WAVeform:FORMat" + std::string(format);
    return Exec(command);
}

/**
 * @brief write to file
 * */
int write_to_file(const char *data, const std::string &file_path) {
    std::string buffer(data);
    std::ofstream file;
    file.open(file_path);
    if (file.is_open()) {
        file << buffer;
        file.close();
        return 0;
    }
    std::cout << "File open failed!\n";
    return 1;
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
    std::string command = "WAVeform:DATA?";
    int num = get_waveform_points();
    int data_length = 10 + num + 1;  // 10 is the length of <header>, 1 is the end breakline(\n)
    char buffer[data_length];
    Exec(command, buffer, true, data_length);
    memcpy(data, buffer + 10, num);
    return 0;
}

/**
 * @brief convert a measurement data array to a 2d array: time array & voltage array
 * */
int KST3000::get_real_data(double **result) {
    int points = get_waveform_points();
    char data[points];
    get_waveform_data(data);
    char preamble[1024];
    get_waveform_preamble(preamble);
    std::string delimiter = ",";
    std::vector<std::string> v_preamble = split(preamble, delimiter);
    double x_increment = std::stod(v_preamble[4]);
    double x_origin = std::stod(v_preamble[5]);
    double x_reference = std::stod(v_preamble[6]);
    double y_increment = std::stod(v_preamble[7]);
    double y_origin = std::stod(v_preamble[8]);
    double y_reference = std::stod(v_preamble[9]);

    for (int i = 0; i < points; i++) {
        double time = ((i - x_reference) * x_increment) + x_origin;
        result[0][i] = time;

        int voltage_data = (int) (unsigned char) data[i];
        if (voltage_data == 0) {
            // Hole. Holes are locations where data has not yet been acquired.
            continue;
        }
        double voltage = ((voltage_data - y_reference) * y_increment) + y_origin;
        result[1][i] = voltage;
    }
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
int KST3000::save_waveform_data(std::string file_path) {
    char preamble[1024];
    get_waveform_preamble(preamble);
    std::string delimiter = ",";
    std::vector<std::string> v_preamble = split(preamble, delimiter);
    int points = get_waveform_points();

    double *result[2];
    result[0] = new double[points];
    result[1] = new double[points];
    get_real_data(result);
    std::stringstream stream;
    stream << "time(ms)" << "," << "voltage(V)" << std::endl;
    for (int i = 0; i < points; i++) {
        stream << result[0][i] * 1000 << "," << result[1][i] << std::endl;
    }
    write_to_file(stream.str().c_str(), file_path);

//  stringstream stream;
//  stream << "time(ms)" << "," << "voltage(V)" << endl;
//  for (int i = 0; i < points; i++) {
//    double time = ((i - x_reference) * x_increment) + x_origin;
//    int voltage_data = (int) (unsigned char) data[i];
//
//    if (voltage_data == 0) {
//      continue;
//    }
//    double voltage = ((voltage_data - y_reference) * y_increment) + y_origin;
//    stream << time * 1000 << "," << voltage << endl;
//  }
//
//  write_to_file(stream.str(), file_path);
    return 0;
}

/**
 * @brief set timebase mode
 * @param mode: {MAIN | WIND | XY | ROLL}
 * must be MAIN before executing digitize
 * */
int KST3000::set_timebase_mode(const char *mode) {
    return Exec("TIMebase:MODE " + std::string(mode));
}

/**
 * @brief capture data
 * */
int KST3000::digitize() {
    return Exec("DIGitize");
}

/**
 * @brief get system setup
 * */
int KST3000::get_system_setup(char *buffer) {
    return Exec("SYSTem:SETup?", buffer);
}

/**
 * @brief set waveform source
 * */
int KST3000::set_waveform_source(int channel) {
    return Exec("WAVeform:SOURce CHANnel" + std::to_string(channel));
}


/**/