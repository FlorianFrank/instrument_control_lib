//
// Created by liuwuhao on 28.09.21.
//

#include "kst3000.h"
#include "kst33500.h"

#ifndef CE_DEVICE_UTILS_H
#define CE_DEVICE_UTILS_H
#endif

#ifndef CHIP
#define CHIP 2
#endif //CE_DEVICE_UTILS_H

#ifndef CELL
#define CELL 16
#endif

#ifndef APP_TO_FILE
#define APP_TO_FILE 1
#endif

#ifndef FREQUENCY
#define FREQUENCY 100
#endif

#ifndef THRESHOLD
#define THRESHOLD 1.2
#endif

void sleep(double secs);

KST33500 connect_wave_generator();

KST3000 connect_oscilloscope();

int config(KST3000 o);

int capture_data(string file_name, KST3000 o);

int save_meta(string file_name, int frequency, double voltage, int cell, int chip = CHIP, bool del_repeat_node = true);

int del_meta_tests();

int set_perfect_sin(KST33500 wg);

int measure_resistance(KST33500 wg, KST3000 o, double measure_v);

int set_measurement_pulse(KST33500 wg, double voltage);

int set_conductive_minus_pulse(KST33500 wg);

string get_main_dir();