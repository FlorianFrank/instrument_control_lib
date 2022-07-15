//
// Created by liuwuhao on 28.09.21.
//
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "pugixml.hpp"
#include "devices/KST3000.h"
#include "devices/KST33500.h"
#include "utils.h"
//#include <boost/filesystem.hpp>


using namespace std;
using namespace pugi;

string PWD = "/Users/lwh/CLionProjects/ce_device/test/";
string DATA_PATH = "/Users/lwh/CLionProjects/ce_device/test/data/";

// the max voltage of the chip can be input, unit /10v, here is 1.2V
int MAX_VOLTAGE = 12;
// the min voltage of the chip can be input, unit /10v, here is 0.1V
int MIN_VOLTAGE = 12;

int MAX_FREQUENCY = 100;
int MIN_FREQUENCY = 100;

double CYCLES_PER_FILE = 1;

//int CHIP = 2;

// how many time a cell need to be tested
int MAX_TIMES = 1;

void sleep(double secs) {
  unsigned int microsecond = 1000000;
  usleep(secs * microsecond);//sleeps for 3 second
}

KST33500 connect_wave_generator() {
  char *wave_generator_ip = "132.231.14.168";
  KST33500 wg = KST33500(wave_generator_ip);
  wg.connect();
  return wg;
}

KST3000 connect_oscilloscope() {
  char *ip = "132.231.14.169";
  KST3000 k = KST3000(ip);
  k.connect();
  return k;
}

/**
 * @brief set params of oscilloscope & waveform generator
 * because every cell has different device parameter to get a better "shape"
 * */
int set_params(KST33500 wg, KST3000 o, int frequency, int voltage) {
  double d_voltage = voltage / 10.0;
  wg.voltage(d_voltage);
  wg.frequency(frequency);
  o.set_channel_range(d_voltage * 2, 1); // *2 for sin wave
  o.set_channel_range(d_voltage * 2, 2); // *2 for sin wave
  o.set_time_range(CYCLES_PER_FILE / frequency); // collect 1 cycle every time
}

/**
 * @brief run a test
 * */
int run_test(int cell, KST33500 wg, KST3000 o, int v) {
  if (v > 12) {
    return 1;
  }
  double voltage = v / 10.0;
  wg.voltage(voltage);

  string file_prefix = DATA_PATH + to_string(cell);

  o.digitize();
  o.set_waveform_source(1);
  o.save_waveform_data(file_prefix + "_1");
  o.set_waveform_source(2);
  o.save_waveform_data(file_prefix + "_2");
  return 0;
}


/**
 * @brief save test meta data
 * */
int save_meta(string file_name, int frequency, double voltage, int cell, int chip, bool del_repeat_node) {
  xml_document doc;
  string meta_file = DATA_PATH + "meta.xml";
  xml_parse_result result = doc.load_file(meta_file.c_str());
  xml_node tests = doc.child("tests");
  if (!tests) {
    tests = doc.append_child("tests");
  }

  string dup_xpath = "//test[contains(file,'" + file_name + "')]";
  xpath_node dup_test = doc.select_node(dup_xpath.c_str());
  if (dup_test && del_repeat_node) {
    dup_test.parent().remove_child(dup_test.node());
  }
  if (dup_test && !del_repeat_node) {
    xml_node cycles_node = dup_test.node().child("cycles");
    int cycles_num = cycles_node.text().as_int() + 1;
    cycles_node.text().set(to_string(cycles_num).c_str());
  }
  if (!dup_test || (dup_test && del_repeat_node)) {
    xml_node test = tests.append_child("test");
    xml_node chip_node = test.append_child("chip");
    chip_node.append_child(node_pcdata).set_value(to_string(chip).c_str());
    xml_node cell_node = test.append_child("cell");
    cell_node.append_child(node_pcdata).set_value(to_string(cell).c_str());
    xml_node frequency_node = test.append_child("frequency");
    frequency_node.append_child(node_pcdata).set_value(to_string(frequency).c_str());
    xml_node voltage_node = test.append_child("voltage");
    voltage_node.append_child(node_pcdata).set_value(to_string(voltage).c_str());
    xml_node file_node = test.append_child("file");
    file_node.append_child(node_pcdata).set_value(file_name.c_str());
//    xml_node cycles_node = test.append_child("cycles");
//    cycles_node.append_child(node_pcdata).set_value(to_string(1).c_str());
  }


  doc.save_file(meta_file.c_str());
  return 0;
}

int stream_triple(stringstream &stream, double time, double input, double output) {
  stream << time << setw(20) << input << setw(20) << output << endl;
  return 0;
}

int write_to_file(string data, string file_path) {
  string buffer(data);
  ofstream file;
  if (APP_TO_FILE == 1) {
    file.open(file_path, ios_base::app);
  } else {
    file.open(file_path);
  }
  if (file.is_open()) {
    file << buffer;
    file.close();
    return 0;
  }
  cout << "File open failed!\n";
  return 1;
}

/**
 * @brief save data into a file containing 3 columns: time; input; output;
 * */
int capture_data(string file_name, KST3000 o) {
  o.digitize();
  int points = o.get_waveform_points();

  o.set_waveform_source(1);
  double *input[2];
  input[0] = new double[points];
  input[1] = new double[points];
  o.get_real_data(input);

  o.set_waveform_source(2);
  double *output[2];
  output[0] = new double[points];
  output[1] = new double[points];
  o.get_real_data(output);

  stringstream stream;
  stream << "time" << "," << "input" << "," << "output" << endl;
  for (int i = 0; i < points; i++) {
    stream << input[0][i] * 1000 << "," << input[1][i] << "," << output[1][i] << endl;
  }
  write_to_file(stream.str(), file_name);
}

/**
 * @brief run a single test
 * @param wg, KST33500
 * @param k, KST3000
 * @param cell, cell number
 * @param frequency, unit: Hz
 * @param voltage, unit: /10 V
 * filename consists of 4 number, separated by a underscore
 * the 4 numbers means:
 *  1, chip
 *  2, cell;
 *  3, frequency
 *  4, voltage(unit: /10 V);
 *  5, timestamp
 * */
int single_test(int cell, KST33500 wg, KST3000 o, int frequency, int i_voltage) {
  double voltage = i_voltage / 10.0;
//  time_t t = time(nullptr);
  wg.voltage(voltage);
  wg.frequency(frequency);
  string file_name = DATA_PATH + to_string(CHIP)
                     + "_" + to_string(cell)
                     + "_" + to_string(frequency)
                     + "_" + to_string(i_voltage);
//                     + "_" + to_string(t);
  capture_data(file_name, o);
  save_meta(file_name, frequency, voltage, cell);
}

/**
 * @brief run a test for a cell
 * filename consists of 4 number, separated by a underscore
 * the 4 numbers means:
 *  1, cell;
 *  2, voltage(unit: /10 V);
 *  3, the nth time test, every cell need to be tested many times
 *  4, frequency
 * */
int test_cell(int cell, KST33500 wg, KST3000 o) {
  for (int f = MIN_FREQUENCY; f <= MAX_FREQUENCY; f += 100) {
    for (int v = MIN_VOLTAGE; v <= MAX_VOLTAGE; v++) {
      set_params(wg, o, f, v);
      sleep(0.1);
      single_test(cell, wg, o, f, v);
    }
  }
}

int config(KST3000 o) {
  o.set_timebase_mode();
  o.set_waveform_points_mode("NORmal");
  o.set_waveform_points(1000);
}

/**
 * clear meta test tags
 * */
int del_meta_tests() {
  xml_document doc;
  string meta_file = DATA_PATH + "meta.xml";
  doc.load_file(meta_file.c_str());
  xml_node tests = doc.child("tests");
  tests.remove_children();
  doc.save_file(meta_file.c_str());
  return 0;
}


/**
 * set a sin waveform oscillator can show perfect shape
 * */
int set_perfect_sin(KST33500 wg) {
  wg.function("SIN");
  wg.voltage(0.8);
  wg.offset(0);
  wg.frequency(100);
//  wg.output(true);
}


/**
 * delete test files
 * */
int del_test_files() {
//  std::remove();
}


/**
 * set -1.2V voltage by pulse waveform(making pulse width short enough to be ignored)
 * */
int set_conductive_minus_pulse(KST33500 wg) {
  wg.function("PULS");
  wg.voltage(0.001);
  // notice offset set 2* value of argument
  wg.offset(-(THRESHOLD / 2));
  wg.set_pulse_width(0.001);
}

/**
 * set target voltage by pulse waveform
 * */
int set_measurement_pulse(KST33500 wg, double voltage) {
  wg.function("PULS");
  wg.voltage(0.001);
  // notice offset set 2* value of argument
  wg.offset(voltage / 2);
  wg.set_pulse_width(0.001);
}

int measure_resistance(KST33500 wg, KST3000 o, double measure_v) {
  // set range of oscillator to make it easy to look
  o.set_channel_range(THRESHOLD * 2.5, 1);
  o.set_channel_range(THRESHOLD * 2.5, 2);
  o.run();
  // TODO: frequency shouldn't matter for now, but maybe need to be confirm
  wg.frequency(FREQUENCY);
  set_conductive_minus_pulse(wg);
  wg.output(true);
  // keep threshold voltage for a while to make the electrons movement happens
  // TODO: this time could also matter
  sleep(0.5);
  wg.output(false);
  // The electrons movement finished by now

  // set range of oscillator to make it easy to look
  o.set_channel_range(measure_v * 2.5, 1);
  o.set_channel_range(measure_v * 2.5, 2);

  set_measurement_pulse(wg, measure_v);
  wg.output(true);
  // keep measurement voltage for a while, otherwise you could capture all 0 as output
  sleep(0.5);
  return 0;
}


string get_main_dir() {
  string file_path = __FILE__;
  string dir_path = file_path.substr(0, file_path.rfind("/"));
  return dir_path;

}