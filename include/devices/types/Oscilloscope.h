//
// Created by florianfrank on 19.07.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_OSCILLOSCOPE_H
#define INSTRUMENT_CONTROL_LIB_OSCILLOSCOPE_H

#include "Device.h"

class Oscilloscope : public Device
{
public:
    enum OSC_CHANNEL
    {
        CHANNEL_1 = 0, CHANNEL_2 = 1, CHANNEL_3 = 2, CHANNEL_4 = 3
    };

    enum TRIGGER_EDGE
    {
        POS_EDGE = 0,
        NEG_EDGE = 1,
        EITHER = 2,
        ALTERNATING = 3
    };

    enum DISPLAY_MODES {
        MAIN = 0,
        WIND = 1,
        XY = 2,
        ROLL = 3
    };

    enum FILE_FORMAT {
        ASCII = 0,
        WORD =   1,
        BYTE =   2,
    };



    Oscilloscope(const char *ip, int timeoutInMs, PIL::Logging *logger);

    virtual int run() = 0;
    virtual int stop() = 0;
    virtual int single() = 0;

    virtual int autoScale() = 0;

    virtual int setTimeRange(double value) = 0;
    virtual int setChannelOffset(OSC_CHANNEL channel, double offset) = 0;
    virtual int setChannelScale(OSC_CHANNEL channel, double value) = 0;
    virtual int setChannelRange(OSC_CHANNEL channel, double value, bool is_v) = 0;


    virtual int setTriggerEdge(TRIGGER_EDGE edge) = 0;
    virtual int setTriggerSource(OSC_CHANNEL channel) = 0;
};


#endif //INSTRUMENT_CONTROL_LIB_OSCILLOSCOPE_H
