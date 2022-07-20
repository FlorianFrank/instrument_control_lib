//
// Created by liuwuhao on 17.06.21.
//

#ifndef CE_DEVICE_KST3000_H
#define CE_DEVICE_KST3000_H

#include "types/Oscilloscope.h"

namespace PIL {
    class Logging;
}

class KST3000 : public Oscilloscope {

public:
    // TODO: timeoutInMS -> Deice
    explicit KST3000(const char *ip, int timeoutInMS);
    KST3000(const char *ip, int timeoutInMs, PIL::Logging *logger);

    int run() override;
    int stop() override;
    int single() override;
    int autoScale() override;

    int setTimeRange(double value) override;
    int setChannelOffset(OSC_CHANNEL channel, double offset) override;
    int setChannelScale(OSC_CHANNEL channel, double value) override;
    int setChannelRange(OSC_CHANNEL channel, double value, bool is_v = true) override; // TODO what is is_v?

    int setTriggerEdge(TRIGGER_EDGE edge) override;
    int setTriggerSource(OSC_CHANNEL channel) override;

    int setTimeDelay(double delay);

    int setWaveformSource(OSC_CHANNEL channel);
    int getWaveformPreamble(char *preamble);
    int getWaveformPoints();
    int setWaveformPoints(int num_points);
    int setWaveformPointsMode(std::string &mode);
    int setWaveformFormat(FILE_FORMAT format);
    int saveWaveformData(std::string file_path = "./buffer");

    int getWaveformData(char *data);
    int getRealData(double **result);
    int digitize();

    int getSystemSetup(char *buffer);
    int setDisplayMode(DISPLAY_MODES displayMode);
    int displayConnection();
    int setChannelDisplay(OSC_CHANNEL channel, int on);

private:
    // Helper-functions
    static std::string getTriggerEdgeStr(TRIGGER_EDGE edge) ;
    static std::string getChannelFromEnum(OSC_CHANNEL channel) ;
    static std::string getDisplayModeFromEnum(DISPLAY_MODES displayMode);
    static std::string getFileFormatStrFromEnum(FILE_FORMAT format);
};

#endif //CE_DEVICE_KST3000_H
