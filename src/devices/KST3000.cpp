/**
 * @brief Implementation of the Keighsight 3000 series oscilloscope interface.
 * @authors Wuhao Liu, Florian Frank
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include "devices/KST3000.h"
#include <unistd.h>

#define SLEEP_DISPLAY_CONNECTION 2 // seconds

/**
 * @brief Constructor
 * @param ip: IP address of kst3000
 * @note Get oscillator's m_IPAddr: Press "Utility" key; Press I/O;
 * */
KST3000::KST3000(const char *ip, int timeoutInMS)
        : Oscilloscope(ip, timeoutInMS, nullptr) {
    m_Logger = new PIL::Logging(PIL::INFO, nullptr);
    //m_DeviceName = DEVICE_NAME;
}


KST3000::KST3000(const char *ip, int timeoutInMS, PIL::Logging *logger)
        : Oscilloscope(ip, timeoutInMS, logger) {
    //m_DeviceName = DEVICE_NAME;
}

PIL_ERROR_CODE KST3000::Exec2(const std::string &command, ExecArgs *args, std::string *result, bool br) {
    auto ret = Exec(command, args);

    if (ret != PIL_NO_ERROR) {
        return ret;
    }

    return Exec(" ", nullptr, result, br);
}


/**
 * @brief Display a line of text: Connected Successfully. Returning...
 *        The message will disappear after 2 seconds.
 *        Can be used to make sure the connection is working.
 * */

PIL_ERROR_CODE KST3000::displayConnection() {
    SubArg subArg("DISP");
    subArg.AddElem("TEXT", ":");

    ExecArgs args;
    args.AddArgument(subArg, "'Connected Successfully. Returning...'", " ");

    auto ret = Exec("", &args);
    if (ret != PIL_NO_ERROR)
        return ret;

    sleep(SLEEP_DISPLAY_CONNECTION); // TODO no numberic values direcly in the source code.

    ExecArgs argsDisp;
    argsDisp.AddArgument("DISP", "ON", " ");
    ret = Exec("", &argsDisp);
    if (ret != PIL_NO_ERROR)
        return ret;

    SubArg subArgClear("DISPlay");
    subArgClear.AddElem("TEXT", ":")
            .AddElem("CLEar", ":");

    ExecArgs argsClear;
    argsClear.AddArgument(subArgClear, "");

    return Exec("", &argsClear);
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
    SubArg triggerArg("TRIGger");
    triggerArg.AddElem("SLOPe", ":");

    ExecArgs args;
    args.AddArgument(triggerArg, getTriggerEdgeStr(edge), " ");

    return Exec("", &args);
}


/**
 * @brief Set trigger  channel
 * @param channel: {1(default) | 2 | 3 | 4}
 * */
PIL_ERROR_CODE KST3000::setTriggerSource(OSC_CHANNEL channel) {
    SubArg subArg("TRIGger");
    subArg.AddElem("SOURce", ":")
            .AddElem("CHAN");

    ExecArgs args;
    args.AddArgument(subArg, getChannelFromEnum(channel));

    return Exec("", &args);
}

/**
 * @brief Set timebase(horizontal) range; Equivalent to adjust the "Horizontal" knob.
 * @param range: time range to set, unit: second
 * */
PIL_ERROR_CODE KST3000::setTimeRange(double range) {
    SubArg subArg("TIMebase");
    subArg.AddElem("RANGe", ":");

    ExecArgs args;
    args.AddArgument(subArg, range, " ");

    return Exec("", &args);
}

/**
 * @brief Set timebase(horizontal) delay; Equivalent to adjust the delay knob in Horizontal controls.
 * e.g. TIMebase:DElay 5
 * @param delay: time delay, unit seconds
 * */
PIL_ERROR_CODE KST3000::setTimeDelay(double delay) {
    SubArg subArg("TIMebase");
    subArg.AddElem("DELay", ":");

    ExecArgs args;
    args.AddArgument(subArg, delay, " ");

    return Exec("", &args);
}

/**
 * @brief Equivalent to adjust the knob above channel numbers.
 *          e.g. CHANnel3:SCALe 5
 * @param scale: units(voltage) per division(grid)
 * @param channel:  target channel: default 1
 * */
PIL_ERROR_CODE KST3000::setChannelScale(OSC_CHANNEL channel, double value) {
    // set the vertical scale
    SubArg subArg("CHANnel");
    subArg.AddElem(channel);
    subArg.AddElem("SCALe", ":");

    ExecArgs args;
    args.AddArgument(subArg, value, " ");

    return Exec("", &args);
}

/**
 * @brief Similar to setChannelScale, but set the whole voltage range.
 * @param range: voltage range(vertical)
 * @param target: channel: default 1
 * @param is_v: voltage unit:
 *              - true: use V as unit, default
 *              - false: use mV as unit
 * */
PIL_ERROR_CODE KST3000::setChannelRange(OSC_CHANNEL channel, double range, VOLTAGE_UNIT voltageUnit) {
    // set the vertical range
    SubArg subArg("CHANnel");
    subArg.AddElem(getChannelFromEnum(channel))
            .AddElem("RANGe", ":");

    ExecArgs args;
    args.AddArgument(subArg, range, " ");

    if (voltageUnit == MILLI_VOLT)
        args.AddArgument("", "mV");
    return Exec("", &args);
}

/**
 * @brief Set vertical offset. Equivalent to adjust the knob below channel number button
 * @param offset:  vertical offset, unit: V
 * @param channel: channel number, default 1
 * */
PIL_ERROR_CODE KST3000::setChannelOffset(OSC_CHANNEL channel, double offset) {
    // set the vertical offset
    SubArg subArg("CHANnel");
    subArg.AddElem(getChannelFromEnum(channel))
            .AddElem("offset", ":");

    ExecArgs args;
    args.AddArgument(subArg, offset, " ");

    return Exec("", &args);
}

/**
 * @brief Show or hide channel display. Equivalent to press channel number button.
 * @param on: 1 or 0. 1: show channel single. 0: hide channel single.
 * @param channel: channel number, default 1
 * */
PIL_ERROR_CODE KST3000::setChannelDisplay(OSC_CHANNEL channel, int on) {
    // turns the channel on(1) or off(0)
    SubArg subArg("CHANnel");
    subArg.AddElem(getChannelFromEnum(channel))
            .AddElem("DISPLAY", ":");

    ExecArgs args;
    args.AddArgument(subArg, on, " ");

    return Exec("", &args);
}

std::vector<std::string> split(const char *splitStr, std::string &delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();

    auto stringToSplit = std::string(splitStr);
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
 * @brief Query the preamble of waveform data, e.g. WAVeform:PREamble?
 * @return a array containing preamble
 * [Waveform format, Acquire type, Waveform points desired, Waveform average count,
 *  Waveform X increment, Waveform X origin, Waveform X reference,
 *  Waveform Y increment, Waveform Y origin, Waveform Y reference]
 * */
PIL_ERROR_CODE KST3000::getWaveformPreamble(std::string *result) {
    SubArg subArg("WAVeform");
    subArg.AddElem("PREamble", ":", "?");

    ExecArgs args;
    args.AddArgument(subArg, "");

    return Exec2("", &args, result, false);
}

/**
 * @brief Query the number of waveform points to be transferred
 * @return the number of waveform points to be transferred
 * */
PIL_ERROR_CODE KST3000::getWaveformPoints(int *nrWaveformPoints) {
    if (!nrWaveformPoints) {
        if (m_EnableExceptions)
            throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
        return PIL_INVALID_ARGUMENTS;
    }

    SubArg subArg("WAVeform");
    subArg.AddElem("POINts", ":", "?");
    // TODO sovle that
    ExecArgs args;
    args.AddArgument("WAVeform", ":POINts?");

    std::string response;
    auto ret = Exec2("", &args, &response, false);
    if (ret != PIL_NO_ERROR)
        return ret;

    *nrWaveformPoints = std::stoi(response);
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
    SubArg subArg("WAVeform");
    subArg.AddElem("POINts", ":")
            .AddElem("MODE", ":");
    ExecArgs args;
    args.AddArgument(subArg, mode, " ");
    return Exec("", &args);
}

/**
 * @brief set number of waveform points
 * */
PIL_ERROR_CODE KST3000::setWaveformPoints(int num_points) {
    SubArg subArg("WAVeform");
    subArg.AddElem("POINts", ":");

    ExecArgs args;
    args.AddArgument(subArg, num_points, " ");
    return Exec("", &args);
}

/**
 * @brief set format of waveform data(default "BYTE")
 * */
PIL_ERROR_CODE KST3000::setWaveformFormat(FILE_FORMAT format) {
    SubArg subArg("WAVeform");
    subArg.AddElem("FORMat", ":");
    ExecArgs args;
    args.AddArgument(subArg, getFileFormatStrFromEnum(format), " ");
    return Exec("", &args);
}

/**
 * @brief write to file
 * */
PIL_ERROR_CODE KST3000::writeToFile(const char *data, const std::string &file_path) {
    std::string buffer(data);
    std::ofstream file;
    file.open(file_path); // TODO make more precise error checks
    if (file.is_open()) {
        file << buffer;
        file.close();
        return PIL_NO_ERROR;
    }

    if (m_EnableExceptions)
        throw PIL::Exception(PIL_INTERFACE_CLOSED, __FILENAME__, __LINE__, "");
    return PIL_INTERFACE_CLOSED;
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
PIL_ERROR_CODE KST3000::getWaveformData(std::string *data) {
    if (!data) {
        if (m_EnableExceptions)
            throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
        return PIL_INVALID_ARGUMENTS;
    }

    SubArg subArg("WAVeform");
    subArg.AddElem("DATA", ":", "?");

    ExecArgs args;
    args.AddArgument(subArg, "");

    int num;
    auto retGetWaveFormPoints = getWaveformPoints(&num);
    if (retGetWaveFormPoints != PIL_NO_ERROR)
        return retGetWaveFormPoints;

    auto execRet = Exec2("", &args, data, false);
    if (execRet != PIL_NO_ERROR) {
        return execRet;
    }

    *data = data->substr(10, data->length());

    return PIL_NO_ERROR;
}

/**
 * @brief convert a measurement data array to a 2d array: time array & voltage array
 * */
PIL_ERROR_CODE KST3000::getRealData(double **result) {
    if (!result) {
        if (m_EnableExceptions)
            throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "");
        return PIL_INVALID_ARGUMENTS;
    }

    int points;
    auto getWaveFormPointsRet = getWaveformPoints(&points);
    if (getWaveFormPointsRet != PIL_NO_ERROR)
        return getWaveFormPointsRet;

    std::string data;
    auto getWaveFormDataRet = getWaveformData(&data);
    if (getWaveFormDataRet != PIL_NO_ERROR)
        return getWaveFormDataRet;

    std::string preamble;
    auto getWaveFormPreambleRet = getWaveformPreamble(&preamble);
    if (getWaveFormPreambleRet != PIL_NO_ERROR)
        return getWaveFormPreambleRet;

    std::string delimiter = ",";
    std::vector<std::string> v_preamble = splitString(preamble, delimiter);
    double x_increment = std::stod(v_preamble[4]);          // TODO add define which makes the position more clear
    double x_origin = std::stod(v_preamble[5]);             // TODO add define which makes the position more clear
    double x_reference = std::stod(v_preamble[6]);          // TODO add define which makes the position more clear
    double y_increment = std::stod(v_preamble[7]);          // TODO add define which makes the position more clear
    double y_origin = std::stod(v_preamble[8]);             // TODO add define which makes the position more clear
    double y_reference = std::stod(v_preamble[9]);          // TODO add define which makes the position more clear

    std::vector<std::string> v_data = splitString(data, delimiter);

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
 * @brief convert a measurement data array to a 2d array: time array & voltage array
 * */
std::vector<std::vector<double>> KST3000::getRealDataPy() {
    int points;
    auto getWaveFormPointsRet = getWaveformPoints(&points);
    if (getWaveFormPointsRet != PIL_NO_ERROR)
        throw PIL::Exception(getWaveFormPointsRet, __FILENAME__, __LINE__, "Failed getting waveform points");

    std::string data;
    auto getWaveFormDataRet = getWaveformData(&data);
    if (getWaveFormDataRet != PIL_NO_ERROR)
        throw PIL::Exception(getWaveFormDataRet, __FILENAME__, __LINE__, "Failed getting waveform data.");

    std::string preamble;
    auto getWaveFormPreambleRet = getWaveformPreamble(&preamble);
    if (getWaveFormPreambleRet != PIL_NO_ERROR)
        throw PIL::Exception(getWaveFormPreambleRet, __FILENAME__, __LINE__, "Failed getting waveform preamble.");

    std::string delimiter = ",";
    std::vector<std::string> v_preamble = splitString(preamble, delimiter);
    double x_increment = std::stod(v_preamble[4]);          // TODO add define which makes the position more clear
    double x_origin = std::stod(v_preamble[5]);             // TODO add define which makes the position more clear
    double x_reference = std::stod(v_preamble[6]);          // TODO add define which makes the position more clear
    double y_increment = std::stod(v_preamble[7]);          // TODO add define which makes the position more clear
    double y_origin = std::stod(v_preamble[8]);             // TODO add define which makes the position more clear
    double y_reference = std::stod(v_preamble[9]);          // TODO add define which makes the position more clear

    std::vector<std::string> v_data = splitString(data, delimiter);

    std::vector<std::vector<double>> result;
    std::vector<double> r1;
    std::vector<double> r2;
    result.push_back(r1);
    result.push_back(r2);

    // TODO make the calculation more clear!
    for (int i = 0; i < points; i++) {
        int voltage_data = (int) (unsigned char) data[i];
        if (voltage_data == 0) {
            // Hole. Holes are locations where data has not yet been acquired.
            continue;
        }

        double time = ((i - x_reference) * x_increment) + x_origin;
        result[0].push_back(time);

        double voltage = ((voltage_data - y_reference) * y_increment) + y_origin;
        result[1].push_back(voltage);
    }

    return result;
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
// TODO refactor to csv handler class
PIL_ERROR_CODE KST3000::saveWaveformData(std::string &file_path) {
    setWaveformFormat(BYTE);
    std::string preamble;
    auto getWaveFormPreambleRet = getWaveformPreamble(&preamble);
    if (getWaveFormPreambleRet != PIL_NO_ERROR)
        return getWaveFormPreambleRet;

    std::string delimiter = ",";
    std::vector<std::string> v_preamble = splitString(preamble, delimiter);
    int points;
    auto getWaveformPointsRet = getWaveformPoints(&points);
    if (getWaveformPointsRet != PIL_NO_ERROR)
        return getWaveformPointsRet;

    double *result[2];
    result[0] = new double[points];
    result[1] = new double[points];
    getRealData(result);
    std::stringstream stream;
    stream << "time(ms)" << "," << "voltage(V)" << std::endl;
    for (int i = 0; i < points; i++) {
        stream << result[0][i] * 1000 << "," << result[1][i]
               << std::endl; // TODO: ADD explanation, no static numeric values in the source code!
    }
    writeToFile(stream.str().c_str(), file_path);
    return PIL_NO_ERROR;
}

/**
 * @brief set timebase mode
 * @param mode: {MAIN | WIND | XY | ROLL}
 * must be MAIN before executing digitize
 * */
PIL_ERROR_CODE KST3000::setDisplayMode(DISPLAY_MODES mode) {
    ExecArgs args;
    args.AddArgument("TIMebase:MODE", getDisplayModeFromEnum(mode), " ");
    return Exec("", &args);
}

/**
 * @brief capture data
 * */
PIL_ERROR_CODE KST3000::digitize(OSC_CHANNEL channel) {
    ExecArgs args;
    return Exec("DIGitize CHAnnel" + getChannelFromEnum(channel), &args);
}

/**
 * @brief get system setup
 * */
PIL_ERROR_CODE KST3000::getSystemSetup(std::string *buffer) {
    SubArg subArg("SYSTem");
    subArg.AddElem("SETup", ":", "?");

    ExecArgs args;
    args.AddArgument(subArg, "");

    return Exec2("", &args, buffer, false);
}

/**
 * @brief set waveform source
 * */
PIL_ERROR_CODE KST3000::setWaveformSource(OSC_CHANNEL channel) {
    ExecArgs execArgs;
    execArgs.AddArgument("CHANnel", getChannelFromEnum(channel));
    return Exec("WAVeform:SOURce ", &execArgs);
}

/*static*/ std::string KST3000::getTriggerEdgeStr(Oscilloscope::TRIGGER_EDGE edge) {
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

/*static*/ std::string KST3000::getChannelFromEnum(Oscilloscope::OSC_CHANNEL channel) {
    switch (channel) {
        case Oscilloscope::CHANNEL_1:
            return "1";
        case Oscilloscope::CHANNEL_2:
            return "2";
        case Oscilloscope::CHANNEL_3:
            return "3";
        case Oscilloscope::CHANNEL_4:
            return "4";
        default:
            if (m_EnableExceptions)
                throw PIL::Exception(PIL_INVALID_ARGUMENTS, __FILENAME__, __LINE__, "Invalid oscilloscope channel");
            return "0"; // TODO check
    }
}

std::string KST3000::getDisplayModeFromEnum(Oscilloscope::DISPLAY_MODES displayMode) {
    switch (displayMode) {
        case MAIN:
            return "MAIN";
        case WIND:
            return "WIND";
        case XY:
            return "XY";
        case ROLL:
            return "ROLL";
    }
    return "MAIN";
}

std::string KST3000::getFileFormatStrFromEnum(Oscilloscope::FILE_FORMAT format) {
    switch (format) {
        case ASCII:
            return "ASCII";
        case WORD:
            return "WORD";
        case BYTE:
            return "BYTE";
    }
    return "BYTE";
}
