//
// Created by liuwuhao on 17.06.21.
//

#ifndef CE_DEVICE_KST3000_H
#define CE_DEVICE_KST3000_H

#include "Device.h"

class KST3000 : public Device {

public:
    explicit KST3000(const char *ip);

    int display_connection();

    int run();

    int stop();

    int single();

    int autoscale();

    int set_trigger_edge(char *);

    int set_trigger_source(int channel = 1);

    int set_time_range(double);

    int set_time_delay(double);

    int set_channel_scale(double, int channel = 1);

    int set_channel_range(double, int channel = 1, bool is_v = true);

    int set_channel_offset(double offset, int channel = 1);

    int set_channel_display(int on, int channel = 1);

    int get_waveform_preamble(char *preamble);

    int get_waveform_points();

    int set_waveform_points(int num_points);

    int set_waveform_points_mode(std::string &mode);

    int set_waveform_format(const char *format = "BYTE");

    int get_waveform_data(char *data);

    int get_real_data(double **result);

    int save_waveform_data(std::string file_path = "./buffer");

    int set_waveform_source(int channel = 1);

    int digitize();

    int set_timebase_mode(const char *mode = "MAIN");

    int get_system_setup(char *buffer);
};

#endif //CE_DEVICE_KST3000_H