/**
 * @brief This file contains the implementation of a communication interface to Keigsight KST300 series oscilloscopes.
 * @authors Wuhao Liu, Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#pragma once

#include "types/Oscilloscope.h"

namespace PIL {
    class Logging;
}

//#define DEVICE_NAME "Mixed Single Oscilloscope" TODO

#define MEASURE_RET_BUFF_SIZE 1024


/**
 * @class KST3000
 * @brief Mixed Single Oscilloscope(Oscillator)
 * Commend operations of a single generator are defined.
 * KST3000 is the model of the device(keysight 3000 series).
 */
class KST3000 : public Oscilloscope
{
public:
    explicit KST3000(const char *ip, int timeoutInMS);
    KST3000(const char *ip, int timeoutInMs, PIL::Logging *logger);

    PIL_ERROR_CODE run() override;
    PIL_ERROR_CODE stop() override;
    PIL_ERROR_CODE single() override;
    PIL_ERROR_CODE autoScale() override;

    PIL_ERROR_CODE setTimeRange(double value) override;
    PIL_ERROR_CODE setChannelOffset(OSC_CHANNEL channel, double offset) override;
    PIL_ERROR_CODE setChannelScale(OSC_CHANNEL channel, double value) override;
    PIL_ERROR_CODE setChannelRange(OSC_CHANNEL channel, double value, VOLTAGE_UNIT voltageUnit) override; // TODO what is is_v?

    PIL_ERROR_CODE setTriggerEdge(TRIGGER_EDGE edge) override;
    PIL_ERROR_CODE setTriggerSource(OSC_CHANNEL channel) override;

    PIL_ERROR_CODE setTimeDelay(double delay);

    PIL_ERROR_CODE setWaveformSource(OSC_CHANNEL channel);
    PIL_ERROR_CODE getWaveformPreamble(std::string *result);
    PIL_ERROR_CODE getWaveformPoints(int* nrWaveFormPoints);
    PIL_ERROR_CODE setWaveformPoints(int num_points);
    PIL_ERROR_CODE setWaveformPointsMode(std::string &mode);
    PIL_ERROR_CODE setWaveformFormat(FILE_FORMAT format);
    PIL_ERROR_CODE saveWaveformData(std::string &file_path); // TODO remove unnecessary default argument

    PIL_ERROR_CODE getWaveformData(std::string *data);
    PIL_ERROR_CODE getRealData(double **result);
    PIL_ERROR_CODE digitize(OSC_CHANNEL channel);

    PIL_ERROR_CODE getSystemSetup(std::string *result);
    PIL_ERROR_CODE setDisplayMode(DISPLAY_MODES displayMode);
    PIL_ERROR_CODE displayConnection();
    PIL_ERROR_CODE setChannelDisplay(OSC_CHANNEL channel, int on);

    PIL_ERROR_CODE Exec2(const std::string &command, ExecArgs *args, std::string *result, bool br);

private:
    // Helper-functions
    static std::string getTriggerEdgeStr(TRIGGER_EDGE edge) ;
    std::string getChannelFromEnum(OSC_CHANNEL channel) ;
    static std::string getDisplayModeFromEnum(DISPLAY_MODES displayMode);
    static std::string getFileFormatStrFromEnum(FILE_FORMAT format);
    PIL_ERROR_CODE writeToFile(const char *data, const std::string &file_path);

};
