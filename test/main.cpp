#include "iostream"
#include "sstream"
#include "utils.h"
#include "KST3000.h"
#include "KST33500.h"
#include "boost/filesystem.hpp"

using namespace std;
using namespace boost::filesystem;


int main() {
  KST33500 wg = connect_wave_generator();
  KST3000 o = connect_oscilloscope();
  o.set_waveform_points(100);

  int times = 10;
  double max_mv = 1.6;
  // TODO: this repeat is for measure several times and get a average;
  //  but maybe what we want is not an average; average could cover some edge situations
  int repeat = 1;
  for (int i = 0; i < times; i++) {
    // every time measurement voltage from 0.1-1.5
    for (double mv = 0.1; mv < max_mv; mv += 0.1) {
      // mv: measurement voltage
      for (int j = 0; j < repeat; j++) {
        // capture repeat times for every cell at every mv
        string cell_dir = get_main_dir()
                          + "/data/stability_formal/"
                          + to_string(CHIP) + "_" + to_string(CELL) + "/";
        if (!exists(cell_dir)) {
          create_directory(cell_dir);
        }
        string times_dir = cell_dir + to_string(i);
        if (!exists(times_dir)) {
          create_directory(times_dir);
        }
        stringstream stream;
        stream << std::fixed << std::setprecision(1) << mv;
        std::string s = stream.str();
        string filename = times_dir + "/"
                          + to_string(CHIP) + "_"
                          + to_string(CELL) + "_"
                          + stream.str()
                          + ".csv";
        measure_resistance(wg, o, mv);
//        string meta_filename = times_dir + "meta.xml";
//        save_meta(filename, FREQUENCY, mv, CELL, CHIP, true);
        capture_data(filename, o);
        sleep(0.5);
      }
    }
  }
  wg.output(false);
  return 0;
}
