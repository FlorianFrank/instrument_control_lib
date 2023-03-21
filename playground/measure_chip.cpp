#include <unistd.h>
#include <iostream>

#include "devices/KST3000.h"
#include "devices/KST33500.h"

using namespace std;

// the max voltage of the chip can be input, unit /10v, here is 1.2V
int MAX_VOLTAGE = 12;
// how many time a cell need to be tested

void sleep(int secs) {
    unsigned int microsecond = 1000000;
    usleep(secs * microsecond);
}

KST33500 connect_wave_generator() {
    const char *wave_generator_ip = "132.231.14.174";
    KST33500 wg = KST33500(wave_generator_ip, 0);
    wg.Connect();
    return wg;
}

KST3000 connect_oscilloscope() {
    const char *ip = "132.231.14.181";
    KST3000 k = KST3000(ip, 0);
    k.Connect();
    return k;
}

/**
 * @brief set params of oscilloscope & waveform generator
 * because every cell has different device parameter to get a better "shape"
 * */
int set_params(int cell, KST33500 wg, KST3000 o) {
    wg.setFrequency(1000);
    o.setTimeRange(1);
    o.setChannelRange(KST3000::CHANNEL_1, 1.6, Oscilloscope::VOLT);
    o.setChannelRange(KST3000::CHANNEL_2, 1.6, Oscilloscope::VOLT);

    // TODO error code
    return 0;
}

/**
 * @brief run test for a cell
 * filename consists of 4 number, separated by a underscore
 * the 4 numbers means:
 *  1, cell;
 *  2, voltage(unit: /10 V);
 *  3, the nth time test, every cell need to be tested many times
 *  4, input/output data, 1 means input data, 2 means output data
 * */
int test_cell(int cell, KST33500 wg, KST3000 o) {
    wg.setFrequency(1000);
    o.setTimeRange(2);
    o.setChannelRange(KST3000::CHANNEL_1, 1.6, Oscilloscope::VOLT);
    o.setChannelRange(KST3000::CHANNEL_2, 1.6, Oscilloscope::VOLT);
    o.setWaveformPoints(1000);

    for (int i = 1; i <= MAX_VOLTAGE; i++) {
        double voltage = i / 10.0;
        wg.setAmplitude(voltage, "");
        string file_prefix = "/home/alexander/Work/Data/memristor/test_data_" + to_string(cell) + "_" + to_string(i);

        std::string data1 = file_prefix + "_1";
        std::string data2 = file_prefix + "_2";

        o.digitize(KST3000::CHANNEL_1);
        o.setWaveformSource(KST3000::CHANNEL_1);
        o.saveWaveformData(data1);
        o.digitize(KST3000::CHANNEL_2);
        o.setWaveformSource(KST3000::CHANNEL_2);
        o.saveWaveformData(data2);
        sleep(1);
    }

    // TODO error code
    return 0;
}

int main() {
    KST33500 wg = connect_wave_generator();

    std::cout << wg.setAmplitude(0.1, "") << std::endl;
    std::cout << wg.turnOn() << std::endl;

    KST3000 o = connect_oscilloscope();
    o.setDisplayMode(KST3000::DISPLAY_MODES::MAIN);
    o.setTimeRange(1);

    test_cell(1, wg, o);

    return 0;
}
