#include <iostream>
#include "utils.h"
#include "kst3000.h"
#include "kst33500.h"

using namespace std;

int FREQUENCY = 100;

int set_conductive_minus_pulse(KST33500 wg) {
  // set -1.2V voltage through making pulse width short enough to be ignored
  wg.function("PULS");
  wg.voltage(0.001);
  wg.offset(+0.6);
  wg.set_pulse_width(0.001);
}

int set_measurement_pulse(KST33500 wg, double voltage) {
  wg.function("PULS");
  wg.voltage(0.001);
  wg.offset(voltage / 2);
  wg.set_pulse_width(0.001);
}

//int set_channels_range(KST3000double voltage) {
//  o.set_channel_range(measure_v * 2.5, 1);
//  o.set_channel_range(measure_v * 2.5, 2);
//}

int measure_once(KST33500 wg, KST3000 o, double measure_v) {

  o.set_channel_range(1.2 * 2.5, 1);
  o.set_channel_range(1.2 * 2.5, 2);
  o.run();
  // frequency shouldn't matter for now
  wg.frequency(FREQUENCY);
  set_conductive_minus_pulse(wg);
  wg.output(true);
  sleep(0.5);
  wg.output(false);

//  sleep(2);
  o.set_channel_range(measure_v * 2.5, 1);
  o.set_channel_range(measure_v * 2.5, 2);

  set_measurement_pulse(wg, measure_v);
  wg.output(true);
//  o.digitize();
  sleep(0.5);


  string filename = "/Users/lwh/CLionProjects/ce_device/test/data/"
                    + to_string(CHIP) + "_"
                    + to_string(CELL) + "_"
                    + to_string((int) (measure_v * 1000));
  save_meta(filename, FREQUENCY, measure_v, CELL, CHIP);
  capture_data(filename, o);
  wg.output(false);
}

int main() {
  KST33500 wg = connect_wave_generator();
  KST3000 o = connect_oscilloscope();
  o.set_waveform_points(100);

  del_meta_tests();
  for (double i = 0.1; i < 1.5; i += 0.1) {
    cout << i << endl;
    measure_once(wg, o, i);
    sleep(0.5);
  }

//  measure_once(wg, o, 0.1);
//  set_perfect_sin(wg);

//  wg.cli();
//  wg.output(true);
//  KST3000 o = connect_oscilloscope();
//  config(o);
//  sleep(1);
//  int cell = 12;
//  for (int i = 0; i < MAX_TIMES; i++) {
//    test_cell(cell, wg, o);
//  }
//  wg.output(false);
  return 0;
}
