#include<unistd.h>

#include "KST3000.h"
#include "KST33500.h"

using namespace std;

// the max voltage of the chip can be input, unit /10v, here is 1.2V
int MAX_VOLTAGE = 12;
// how many time a cell need to be tested

void sleep(int secs) {
  unsigned int microsecond = 1000000;
  usleep(secs * microsecond);//sleeps for 3 second
}

bool connect_wave_generator(KST33500 *waveformGenerator)
{
  const char *wave_generator_ip = "132.231.14.245";
  KST33500 wg = KST33500(wave_generator_ip);
  wg.connect();
  return wg;
}

KST3000 connect_oscilloscope() {
  const char *ip = "132.231.14.243";
  KST3000 k = KST3000(ip);
  k.connect();
  return k;
}

/**
 * @brief set params of oscilloscope & waveform generator
 * because every cell has different device parameter to get a better "shape"
 * */
int set_params(int cell, KST33500 wg, KST3000 o) {
  wg.frequency(1000); // 1000 Hz
  o.set_time_range(1);
  o.set_channel_range(1.6, 1);
  o.set_channel_range(1.6, 2);

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
  set_params(cell, wg, o);

  for (int i = 1; i <= MAX_VOLTAGE; i++) {
    double voltage = i / 10.0;
    wg.voltage(voltage);
    string file_prefix = "/Users/lwh/CLionProjects/ce_device/test/data" + to_string(cell) + "_" + to_string(i) + "_";

    o.digitize();
    o.set_waveform_source(1);
    o.save_waveform_data(file_prefix + "_1");
    o.set_waveform_source(2);
    o.save_waveform_data(file_prefix + "_2");
    sleep(1);
  }

  // TODO error code
  return 0;
}

int main() {
  KST33500 wg = connect_wave_generator();
  KST3000 o = connect_oscilloscope();
  o.set_timebase_mode("MAIN");
  o.set_time_range(1);

//  test_cell(1, wg, o);

  return 0;
}
