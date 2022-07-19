#include <iostream>
#include <fstream>
#include <unistd.h>
#include "devices/KST3000.h"
#include <iomanip>
#include "devices/KST33500.h"
#include "pugixml.hpp"
#include "CommandLineInterface.h"

using namespace pugi;

#define DATA_PATH "/home/florianfrank/Desktop/Measurements/"

// the max voltage of the chip can be input, unit /10v, here is 1.2V
int MAX_VOLTAGE = 12;
// the min voltage of the chip can be input, unit /10v, here is 0.1V
int MIN_VOLTAGE = 12;

int MAX_FREQUENCY = 100;
int MIN_FREQUENCY = 100;

double CYCLES_PER_FILE = 10.0;

int CHIP = 1;

// how many time a cell need to be tested
int MAX_TIMES = 100;

void sleep(double secs) {
  unsigned int microsecond = 1000000;
  usleep(secs * microsecond);//sleeps for 3 second
}

bool connect_wave_generator(KST33500 *waveformGenerator)
{
  if(!waveformGenerator->Connect())
  {
      std::cout << "Error while connecting to the wave_generator (" << waveformGenerator->ReturnErrorMessage() << ")" << std::endl;
      delete waveformGenerator;
      return false;
  }
    return true;
}

bool connect_oscilloscope(KST3000 *oscilloscope)
{
    if (!oscilloscope->Connect())
    {
        std::cout << "Error while connecting to the oscilloscope (" << oscilloscope->ReturnErrorMessage() << ")"
                  << std::endl;
        delete oscilloscope;
        return false;
    }
    return true;
}

/**
 * @brief set params of oscilloscope & waveform generator
 * because every cell has different device parameter to get a better "shape"
 * */
bool set_params(KST33500 *wg, KST3000 *o, int frequency, int voltage) {

    if(!wg || !o)
    {
        std::cout << __LINE__ <<": Error invalid arguments" << std::endl;
        return false;
    }

  double d_voltage = voltage / 10.0;
  if(!wg->setAmplitude(FunctionGenerator::CHANNEL_A, d_voltage, ""))
      std::cout << __LINE__ << ": Error while setting the voltage (" << wg->ReturnErrorMessage() << ")" << std::endl;

  if(!wg->setFrequency(FunctionGenerator::CHANNEL_A, frequency))
  {
      std::cout << __LINE__ << ": Error while setting the frequency " << wg->ReturnErrorMessage() << ")" << std::endl;
      return false;
  }

  if(!o->setChannelRange(Oscilloscope::CHANNEL_1, d_voltage * 2)) // *2 for sin wave
  {
      std::cout << __LINE__ << ": Error while setting the channel_range " << wg->ReturnErrorMessage() << ")"
                << std::endl;
      return false;
  }

  if(!o->setChannelRange(Oscilloscope::CHANNEL_2, d_voltage * 2)) // *2 for sin wave
  {
      std::cout << __LINE__ << ": Error while setting the channel_range " << wg->ReturnErrorMessage() << ")" << std::endl;
      return false;
  }

  if(!o->setTimeRange(CYCLES_PER_FILE / frequency)) // collect 1 cycle every time
  {
      std::cout << __LINE__ << ": Error while setting the time_range " << wg->ReturnErrorMessage() << ")" << std::endl;
      return false;
  }
    return true;
}

/**
 * @brief run a test
 * */
/*int run_test(int cell, KST33500 wg, KST3000 o, int v) {
  if (v > 12) {
    return 1;
  }
  double voltage = v / 10.0;
  wg.voltage(voltage);

  string file_prefix = std::string(DATA_PATH) + to_string(cell);

  o.digitize();
  o.set_waveform_source(1);
  o.save_waveform_data(file_prefix + "_1");
  o.set_waveform_source(2);
  o.save_waveform_data(file_prefix + "_2");

  // TODO error code
  return 0;
}*/


/**
 * @brief save test meta data
 * */
int save_meta(std::string &file_name, int frequency, double voltage, int cell, int chip = 1) {
  xml_document doc;
  std::string meta_file = std::string(DATA_PATH) + "meta.xml";
  xml_parse_result result = doc.load_file(meta_file.c_str());
  if(result.status != status_ok)
  {
      std::cout << "Error while loading document (" << meta_file <<  ") Error: (" << result.description() << ")" << std::endl;
      return -1;
  }
  xml_node tests = doc.child("tests");
  if (!tests) {
    tests = doc.append_child("tests");
  }

  std::string dup_xpath = "//test[contains(file,'" + file_name + "')]";
  xpath_node dup_test = doc.select_node(dup_xpath.c_str());
  if (dup_test) {
    dup_test.node().parent().remove_child(dup_test.node());
  }

  xml_node test = tests.append_child("test");
  xml_node chip_node = test.append_child("chip");
  chip_node.append_child(node_pcdata).set_value(std::to_string(chip).c_str());
  xml_node cell_node = test.append_child("cell");
  cell_node.append_child(node_pcdata).set_value(std::to_string(cell).c_str());
  xml_node frequency_node = test.append_child("frequency");
  frequency_node.append_child(node_pcdata).set_value(std::to_string(frequency).c_str());
  xml_node voltage_node = test.append_child("voltage");
  voltage_node.append_child(node_pcdata).set_value(std::to_string(voltage).c_str());
  xml_node file_node = test.append_child("file");
  file_node.append_child(node_pcdata).set_value(file_name.c_str());

  doc.save_file(meta_file.c_str());
  return 0;
}

int stream_triple(std::stringstream &stream, double time, double input, double output) {
  stream << time << std::setw(20) << input << std::setw(20) << output << std::endl;
  return 0;
}

int write_to_file(const std::string& data, const std::string& file_path) {
  const std::string& buffer(data);
  std::ofstream file;
  file.open(file_path);
  if (file.is_open()) {
    file << buffer;
    file.close();
    return 0;
  }
  std::cout << "File open failed!\n";
  return 1;
}

/**
 * @brief save data into a file containing 3 columns: time; input; output;
 * */
bool capture_data(const std::string& file_name, KST3000 *o) {
    if(!o)
    {
        std::cout << __LINE__ << ": Error invalid arguments" << std::endl;
        return false;
    }

  o->digitize();
  int points = o->getWaveformPoints();

    o->setWaveformSource(1);
  double *input[2];
  input[0] = new double[points];
  input[1] = new double[points];
    o->getRealData(input);

    o->setWaveformSource(2);
  double *output[2];
  output[0] = new double[points];
  output[1] = new double[points];
    o->getRealData(output);

  std::stringstream stream;
  stream << "time" << "," << "input" << "," << "output" << std::endl;
  for (int i = 0; i < points; i++) {
    stream << input[0][i] * 1000 << "," << input[1][i] << "," << output[1][i] << std::endl;
  }
  write_to_file(stream.str(), file_name);
  // TODO error
  return true;
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
bool single_test(int cell, KST33500 *wg, KST3000 *o, int frequency, int i_voltage) {
    if(!wg || !o)
    {
        std::cout << __LINE__ << ": Error invalid arguments" << std::endl;
        return false;
    }

  double voltage = i_voltage / 10.0;
  time_t t = time(nullptr);
  if(!wg->setAmplitude(FunctionGenerator::CHANNEL_A, voltage, ""))
  {
      std::cout << __LINE__ << ": Error while setting the frequency " << wg->ReturnErrorMessage() << ")"
                << std::endl;
      return false;
  }

  if(!wg->setFrequency(FunctionGenerator::CHANNEL_B, frequency))
  {
      std::cout << __LINE__ << ": Error while setting the channel_range " << wg->ReturnErrorMessage() << ")"
                << std::endl;
      return false;
  }

  std::string file_name = DATA_PATH + std::to_string(CHIP)
                     + "_" + std::to_string(cell)
                     + "_" + std::to_string(frequency)
                     + "_" + std::to_string(i_voltage)
                     + "_" + std::to_string(t);

  if(!capture_data(file_name, o))
  {
      std::cout << __LINE__ << ": Error while capturing data " << wg->ReturnErrorMessage() << ")"
                << std::endl;
      return false;
  }

  return save_meta(file_name, frequency, voltage, cell);
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
bool memristor_frequency_test(const int cell, KST33500 *wg, KST3000 *o) {
    if(!wg || !o)
    {
        std::cout << __LINE__ << ": Error invalid arguments" << std::endl;
        return false;
    }

  for (int f = MIN_FREQUENCY; f <= MAX_FREQUENCY; f += 100)
  {
      for (int v = MIN_VOLTAGE; v <= MAX_VOLTAGE; v++)
      {
          for (int i = 2; i <= MAX_VOLTAGE; i++)
          {
              std::cout << "Set voltage to " << i << std::endl;
              if(!set_params(wg, o, f, i))
                  return false;
              sleep(0.5);
          }
          sleep(1.0);
          if(!single_test(cell, wg, o, f, v))
              return false;
      }
  }
  return true;
}



bool config(KST3000 *o) {
    if(!o)
    {
        std::cout << __LINE__ << ": Error invalid arguments" << std::endl;
        return false;
    }

  std::string mode = "NORmal";
  if(!o->setWaveformPointsMode(mode))
  {
      std::cout << __LINE__ << ": Error while setting waveform points mode " << o->ReturnErrorMessage() << ")"
                << std::endl;
      return false;
  }

  if(!o->setWaveformPoints(1000))
  {
      std::cout << __LINE__ << ": Error while setting waveform points " << o->ReturnErrorMessage() << ")"
                    << std::endl;
          return false;
  }

    return true;
}

void testPulses(std::string &args)
{
    auto argumentList = CommandLineInterface::splitArguments(args);
    if(argumentList.empty())
    {
        std::cout << "Error: argument list is empty" << std::endl;
        return;
    }

    const char *wave_generator_ip = "132.231.14.245";
    auto* wg = new KST33500(wave_generator_ip, 0);

    const char *ip = "132.231.14.243";
    auto* o = new KST3000(ip, 0);

    try
    {
        int cellID = std::stoi(argumentList[0]);

        if(!connect_wave_generator(wg))
        {
            std::cout << "Unable to Connect to waveform generator " << std::endl;
            return;
        }
        if(!connect_oscilloscope(o))
        {
            std::cout << "Unable to Connect to the oscilloscope" << std::endl;
            return;
        }
        if(!config(o))
            return;

        printf("Measure cell %d\n", cellID);
        if(!memristor_frequency_test(cellID, wg, o))
            return;
    } catch (std::invalid_argument &e)
    {
        std::cout << "    Error argument is not a valid cell ID " << std::endl;
    }
    delete wg;
    delete o;

    // TODO check
}

void testCellAllFrequencies(std::string &args)
{
    auto argumentList = CommandLineInterface::splitArguments(args);
    if(argumentList.empty())
    {
        std::cout << "Error: argument list is empty" << std::endl;
        return;
    }

    const char *wave_generator_ip = "132.231.14.245";
    auto* wg = new KST33500(wave_generator_ip, 0);

    const char *ip = "132.231.14.243";
    auto* o = new KST3000(ip, 0);

    try
    {
        int cellID = std::stoi(argumentList[0]);

        if(!connect_wave_generator(wg))
        {
            std::cout << "Unable to Connect to waveform generator " << std::endl;
            return;
        }
        if(!connect_oscilloscope(o))
        {
            std::cout << "Unable to Connect to the oscilloscope" << std::endl;
            return;
        }
        if(!config(o))
            return;

        printf("Measure cell %d\n", cellID);
        if(!memristor_frequency_test(cellID, wg, o))
            return;
    } catch (std::invalid_argument &e)
    {
        std::cout << "    Error argument is not a valid cell ID " << std::endl;
    }
    delete wg;
    delete o;
}

int main() {

  CommandLineInterface cli;
  CommandLineInterface::addCustomCommandLineOption("test_all_frequencies", "- Starts a memristor hysterese loop test with different freqnencies",
                                                       testCellAllFrequencies);


    CommandLineInterface::addCustomCommandLineOption("test_all_frequencies", "- Starts a memristor hysterese loop test with different freqnencies",
                                                       testPulses);
  cli.Start();

    return 0;
}
