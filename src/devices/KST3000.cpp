//
// Created by liuwuhao on 17.06.21.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include "devices/KST3000.h"
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
KST3000::KST3000(const char *ip, int timeoutInMS) : Oscilloscope(ip, timeoutInMS, nullptr) {
    this->m_DeviceName = "Mixed Single Oscilloscope";
}

KST3000::KST3000(const char *ip, int timeoutInMS, PIL::Logging *logger) : Oscilloscope(ip, timeoutInMS, logger) {
    this->m_DeviceName = "Mixed Single Oscilloscope";
}


/**
 * @brief Display a line of text: Connected Successfully. Returning...
 *        The message will disappear after 2 seconds.
 *        Can be used to make sure the connection is working.
 * */

PIL_ERROR_CODE KST3000::displayConnection() {
    auto ret = Exec("DISP:TEXT 'Connected Successfully. Returning...'");
    if(ret != PIL_NO_ERROR)
        return ret;

    sleep(2); // TODO no numberic values direcly in the source code.
    ret = Exec("DISP ON");
    if(ret != PIL_NO_ERROR)
        return ret;

    return Exec("DISPlay:TEXT:CLEar");
}

/**
 * @brief Start to run. Equivalent to press the Run/Stop button when the device is not running.
 * */
PIL_ERROR_CODE KST3000::run() {
    return Exec("RUN");
}

/**
 * @brief Stop to run. Equivalent to press the Run/Stop button when the device is not stopping.
 * */
PIL_ERROR_CODE KST3000::stop() {
    return Exec("STOP");
}

/**
 * @brief Equivalent to press the Single button.
 * */
PIL_ERROR_CODE KST3000::single() {
    return Exec("SINGLE");
}

/**
 * @brief Equivalent to press the "Auto Scale" button.
 * */
PIL_ERROR_CODE KST3000::autoScale() {
    return Exec("AUToscale");
}

/**
 * @brief Set trigger slope
 * @param edge: {POS | NEG | EITH | ALT} (Rising | Falling | Either | Alternating)
 * */
PIL_ERROR_CODE KST3000::setTriggerEdge(TRIGGER_EDGE edge) {
    return Exec("TRIGger:SLOPe " + getTriggerEdgeStr(edge));
}

/**
 * @brief Set trigger  channel
 * @param channel: {1(default) | 2 | 3 | 4}
 * */
PIL_ERROR_CODE KST3000::setTriggerSource(OSC_CHANNEL channel) {
    return Exec("TRIGger:SOURce CHAN" + getChannelFromEnum(channel));
}

/**
 * @brief Set timebase(horizontal) range; Equivalent to adjust the "Horizontal" knob.
 * @param range: time range to set, unit: second
 * */
PIL_ERROR_CODE KST3000::setTimeRange(double range) {
    return Exec("TIMebase:RANGe " + std::to_string(range));
}

/**
 * @brief Set timebase(horizontal) delay; Equivalent to adjust the delay knob in Horizontal controls.
 * @param delay: time delay, unit seconds
 * */
PIL_ERROR_CODE KST3000::setTimeDelay(double delay) {
    return Exec("TIMebase:DELay " + std::to_string(delay));
}

/**
 * @brief Equivalent to adjust the knob above channel numbers.
 * @param scale: units(voltage) per division(grid)
 * @param channel:  target channel: default 1
 * */
PIL_ERROR_CODE KST3000::setChannelScale(OSC_CHANNEL channel, double value) {
    // set the vertical scale
    std::string command = ""
                          "CHANnel" + std::to_string(channel) + ":SCALe " + getChannelFromEnum(channel);
    return Exec(command);
}

/**
 * @brief Similar to setChannelScale, but set the whole voltage range.
 * @param range: voltage range(vertical)
 * @param target: channel: default 1
 * @param is_v: voltage unit:
 *              - true: use V as unit, default
 *              - false: use mV as unit
 * */
PIL_ERROR_CODE KST3000::setChannelRange(OSC_CHANNEL channel, double range, bool is_v)
{ // TODO remove this v
    // set the vertical range
    std::string command = "CHANnel" + getChannelFromEnum(channel) + ":RANGe " + std::to_string(range);
    if (!is_v)
    {
        command += " mV"; // What is is_v?
    }
    return Exec(command);
}

/**
 * @brief Set vertical offset. Equivalent to adjust the knob below channel number button
 * @param offset:  vertical offset, unit: V
 * @param channel: channel number, default 1
 * */
PIL_ERROR_CODE KST3000::setChannelOffset(OSC_CHANNEL channel, double offset)
{
    // set the vertical offset
    std::string command = "CHANnel" + getChannelFromEnum(channel) + ":offset " + std::to_string(offset);
    return Exec(command);
}

/**
 * @brief Show or hide channel display. Equivalent to press channel number button.
 * @param on: 1 or 0. 1: show channel single. 0: hide channel single.
 * @param channel: channel number, default 1
 * */
PIL_ERROR_CODE KST3000::setChannelDisplay(OSC_CHANNEL channel, int on) {
    // turns the channel on(1) or off(0)
    std::string command = "CHANnel" + getChannelFromEnum(channel) + ":DISPlay " + std::to_string(on);
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
PIL_ERROR_CODE KST3000::getWaveformPreamble(char *preamble) {
    std::string command = "WAVeform:PREamble?";
    return Exec(command, preamble);
}

/**
 * @brief Query the number of waveform points to be transferred
 * @return the number of waveform points to be transferred
 * */
PIL_ERROR_CODE KST3000::getWaveformPoints(int *nrWaveformPoints)
{
    if (!nrWaveformPoints)
        return PIL_INVALID_ARGUMENTS;

    std::string command = "WAVeform:POINts?";
    char buffer[1024] = {0};
    auto ret = Exec(command, buffer);
    if (ret != PIL_NO_ERROR)
        return ret;

    *nrWaveformPoints = std::stoi(buffer);
    return PIL_NO_ERROR;
}

/**
 * @brief set waveform points mode
 * @param mode:
 *          - NORMal
 *          - MAXimum
 *          - RAW
 * */
PIL_ERROR_CODE KST3000::setWaveformPointsMode(std::string &mode) {
    std::string command = "WAVeform:POINts:MODE " + mode;
    return Exec(command);
}

/**
 * @brief set number of waveform points
 * */
PIL_ERROR_CODE KST3000::setWaveformPoints(int num_points) {
    std::string command = "WAVeform:POINts " + std::to_string(num_points);
    return Exec(command);
}

/**
 * @brief set format of waveform data(default "BYTE")
 * */
PIL_ERROR_CODE KST3000::setWaveformFormat(FILE_FORMAT format) {
    std::string command = "WAVeform:FORMat" + getFileFormatStrFromEnum(format);
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
PIL_ERROR_CODE KST3000::getWaveformData(char *data) {
    if(!data)
        return PIL_INVALID_ARGUMENTS;

    std::string command = "WAVeform:DATA?";
    int num;
    auto retGetWaveFormPoints = getWaveformPoints(&num);
    if(retGetWaveFormPoints != PIL_NO_ERROR)
        return retGetWaveFormPoints;

    int data_length = 10 + num + 1;  //TODO 10 is the length of <header>, 1 is the end breakline(\n)
    char buffer[data_length];
    auto execRet = Exec(command, buffer, true, data_length);
    if(execRet != PIL_NO_ERROR)
        return execRet;

    memcpy(data, buffer + 10, num); // TODO: add check if data is large enough
    return PIL_NO_ERROR;
}

/**
 * @brief convert a measurement data array to a 2d array: time array & voltage array
 * */
PIL_ERROR_CODE KST3000::getRealData(double **result) {
    if(!result)
        return PIL_INVALID_ARGUMENTS;

    int points;
    auto getWaveFormPointsRet = getWaveformPoints(&points);
    if (getWaveFormPointsRet != PIL_NO_ERROR)
        return getWaveFormPointsRet;

    char data[points];
    auto getWaveFormDataRet = getWaveformData(data);
    if(getWaveFormDataRet != PIL_NO_ERROR)
        return getWaveFormDataRet;

    char preamble[1024]; // TODO avoid static array definitions in member functions
    auto getWaveFormPreambleRet = getWaveformPreamble(preamble);
    if(getWaveFormPreambleRet != PIL_NO_ERROR)
        return getWaveFormPreambleRet;

    std::string delimiter = ",";
    std::vector<std::string> v_preamble = split(preamble, delimiter);
    double x_increment = std::stod(v_preamble[4]);          // TODO add define which makes the position more clear
    double x_origin = std::stod(v_preamble[5]);             // TODO add define which makes the position more clear
    double x_reference = std::stod(v_preamble[6]);          // TODO add define which makes the position more clear
    double y_increment = std::stod(v_preamble[7]);          // TODO add define which makes the position more clear
    double y_origin = std::stod(v_preamble[8]);             // TODO add define which makes the position more clear
    double y_reference = std::stod(v_preamble[9]);          // TODO add define which makes the position more clear

    // TODO make the calculation more clear!
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
    return PIL_NO_ERROR;
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
PIL_ERROR_CODE KST3000::saveWaveformData(std::string file_path) { // TODO: avoid call by copy on strings
    char preamble[1024]; // TODO avoid static numeric definitions in source code
    auto getWaveFormPreambleRet = getWaveformPreamble(preamble);
    if(getWaveFormPreambleRet != PIL_NO_ERROR)
        return getWaveFormPreambleRet;

    std::string delimiter = ",";
    std::vector<std::string> v_preamble = split(preamble, delimiter);
    int points;
    auto getWaveformPointsRet = getWaveformPoints(&points);
    if(getWaveformPointsRet != PIL_NO_ERROR)
        return getWaveformPointsRet;

    double *result[2];
    result[0] = new double[points];
    result[1] = new double[points];
    getRealData(result);
    std::stringstream stream;
    stream << "time(ms)" << "," << "voltage(V)" << std::endl;
    for (int i = 0; i < points; i++) {
        stream << result[0][i] * 1000 << "," << result[1][i] << std::endl; // TODO: ADD explanation, no static numeric values in the source code!
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
    return PIL_NO_ERROR;
}

/**
 * @brief set timebase mode
 * @param mode: {MAIN | WIND | XY | ROLL}
 * must be MAIN before executing digitize
 * */
PIL_ERROR_CODE KST3000::setDisplayMode(DISPLAY_MODES mode) {
    return Exec("TIMebase:MODE " + getDisplayModeFromEnum(mode));
}

/**
 * @brief capture data
 * */
PIL_ERROR_CODE KST3000::digitize() {
    return Exec("DIGitize");
}

/**
 * @brief get system setup
 * */
PIL_ERROR_CODE KST3000::getSystemSetup(char *buffer) {
    return Exec("SYSTem:SETup?", buffer);
}

/**
 * @brief set waveform source
 * */
PIL_ERROR_CODE KST3000::setWaveformSource(OSC_CHANNEL channel) {
    return Exec("WAVeform:SOURce CHANnel" + getChannelFromEnum(channel));
}

/*static*/ std::string KST3000::getTriggerEdgeStr(Oscilloscope::TRIGGER_EDGE edge)
{
    switch (edge) {
        case Oscilloscope::POS_EDGE:
            return "POS";
        case Oscilloscope::NEG_EDGE:
            return "NEG";
        case Oscilloscope::EITHER:
            return "EITH";
        case Oscilloscope::ALTERNATING:
            return "ALT";
        default:
            return "POS"; // TODO check
    }
}

/*static*/ std::string KST3000::getChannelFromEnum(Oscilloscope::OSC_CHANNEL channel)
{
    switch (channel)
    {
        case Oscilloscope::CHANNEL_1:
            return "1";
        case Oscilloscope::CHANNEL_2:
            return "2";
        case Oscilloscope::CHANNEL_3:
            return "3";
        case Oscilloscope::CHANNEL_4:
            return "4";
        default:
            return "1"; // TODO check
    }
}

std::string KST3000::getDisplayModeFromEnum(Oscilloscope::DISPLAY_MODES displayMode)
{
    switch (displayMode)
    {
        case MAIN:
            return "MAIN";
        case WIND:
            return "WIND";
        case XY:
            return "XY";
        case ROLL:
            return "ROLL";
        default:
            return "MAIN"; // TODO check
    }
}

std::string KST3000::getFileFormatStrFromEnum(Oscilloscope::FILE_FORMAT format)
{
    switch (format)
    {
        case ASCII:
            return "ASCII";
        case WORD:
            return "WORD";
        case BYTE:
            return "BYTE";
        default:
            return "BYTE"; // TODO check
    }
}


/**/