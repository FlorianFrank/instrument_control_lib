//
// Created by liuwuhao on 17.06.21.
//

#ifndef CE_DEVICE_KST3000_H
#define CE_DEVICE_KST3000_H

#include "device.h"
using namespace std;

class KST3000 : public Device {
public:
    KST3000(char *ip);
    int display_connection();
    int run();
    int stop();
    int single();
    int autoscale();
    int set_time_range(double);
    int set_channel_scale(double, int channel = 1);
    int set_channel_range(double, int channel = 1, bool is_v = true);
    int set_channel_offset(double range, int channel = 1);
    int set_channel_display(int on, int channel = 1);
    int get_waveform_points();
    char *get_waveform_data();
};

#endif //CE_DEVICE_KST3000_H
