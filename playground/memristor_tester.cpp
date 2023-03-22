#include <iostream>
#include <fstream>
#include <unistd.h>
#include <devices/KST3000.h>
#include <iomanip>
#include <thread>
#include "devices/KST33500.h"
#include "CommandLineInterface.h"

// "/mnt/file_server/measure_data/KNOWM_Memristors/Chip1" "//"/home/file_server/measure_data/KNOWM_Memristors/Chip1"
#define DATA_PATH "/home/florianfrank/Desktop/Measurements/Chip1"

#define O_IP ""
#define WG_IP ""
#define TIMEOUT 10000

// the max voltage of the chip can be input, unit /10v, here is 1.2V
int MAX_VOLTAGE = 10;
// the min voltage of the chip can be input, unit /10v, here is 0.1V
int MIN_VOLTAGE = 10;

#define NR_FREQUENCIES 4
int frequencies[]{100, 500, 1000, 10000};
int MIN_FREQUENCY = 100;
int MAX_FREQUENCY = 1000;


double CYCLES_PER_FILE = 5.0;

int CHIP = 1;

// how many time a cell need to be tested
int MAX_TIMES = 100;

void sleepSecs(double secs) {
    unsigned int microsecond = 1000000;
    usleep(secs * microsecond);//sleeps for 3 second
}

bool connect_wave_generator(KST33500 *waveformGenerator) {
    auto ret = waveformGenerator->Connect();
    if (ret != PIL_NO_ERROR) {
        std::cout << "Error while connecting to the wave_generator (" << ret << ")" << std::endl;
        delete waveformGenerator;
        return false;
    }
    std::cout << "Connection to Waveform Generator successful" << std::endl;
    return true;
}

bool connect_oscilloscope(KST3000 *oscilloscope) {
    auto ret = oscilloscope->Connect();
    if (ret != PIL_NO_ERROR) {
        std::cout << "Error while connecting to the oscilloscope (" << ret << ")" << std::endl;
        delete oscilloscope;
        return false;
    }
    std::cout << "Connection to Oscilloscope over ip successful" << std::endl;

    return true;
}

/**
 * @brief set params of oscilloscope & waveform generator
 * because every cell has different device parameter to get a better "shape"
 * */
bool set_params(KST33500 *wg, KST3000 *o, int frequency, int voltage) {

    if (!wg || !o) {
        std::cout << __LINE__ << ": Error invalid arguments" << std::endl;
        return false;
    }

    double d_voltage = voltage / 10.0;
    auto ret = wg->setAmplitude(d_voltage, "");
    if (ret != PIL_NO_ERROR)
        std::cout << __LINE__ << ": Error while setting the voltage (" << ret << ")" << std::endl;

    ret = wg->setFrequency(frequency);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting the frequency " << ret << ")" << std::endl;
        return false;
    }

    ret = o->setChannelRange(Oscilloscope::CHANNEL_1, d_voltage * 2, Oscilloscope::VOLT);
    if (ret != PIL_NO_ERROR) // *2 for sin wave
    {
        std::cout << __LINE__ << ": Error while setting the channel_range " << ret << ")"
                  << std::endl;
        return false;
    }

    ret = o->setChannelRange(Oscilloscope::CHANNEL_2, d_voltage * 2, Oscilloscope::VOLT);
    if (ret != PIL_NO_ERROR) // *2 for sin wave
    {
        std::cout << __LINE__ << ": Error while setting the channel_range " << ret << ")" << std::endl;
        return false;
    }


    ret = o->setTimeRange(10.0 / frequency);
    if (ret != PIL_NO_ERROR) // collect 1 cycle every time
    {
        std::cout << __LINE__ << ": Error while setting the time_range " << ret << ")" << std::endl;
        //return false;
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


int stream_triple(std::stringstream &stream, double time, double input, double output) {
    stream << time << std::setw(20) << input << std::setw(20) << output << std::endl;
    return 0;
}

int write_to_file(const std::string &data, const std::string &file_path) {
    const std::string &buffer(data);
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
bool capture_data(const std::string &file_name, KST3000 *o, double frequency) {
    if (!o) {
        std::cout << __LINE__ << ": Error invalid arguments" << std::endl;
        return false;
    }
    std::stringstream stream;
    stream << "time" << "," << "input" << "," << "output" << std::endl;


    for (int i = 0; i < 20; i++) {
        printf("Test execution status: %i%%", i);
        printf("\r");
        fflush(stdout);
        //usleep(((1.0/frequency) * CYCLES_PER_FILE) * 1e6);t


        int points;
        o->getWaveformPoints(&points);
        if (points == 0) {
            std::cout << "Available points is 0 Return from function" << std::endl;
            i - 1;
            continue;
        }

        o->digitize(KST3000::CHANNEL_1);
        o->setWaveformSource(KST3000::CHANNEL_1);
        double *input[2];
        input[0] = new double[points];
        input[1] = new double[points];
        int ch1Ret = o->getRealData(input);

        o->digitize(KST3000::CHANNEL_2);
        o->setWaveformSource(KST3000::CHANNEL_2);
        double *output[2];
        output[0] = new double[points];
        output[1] = new double[points];
        int ch2Ret = o->getRealData(output);

        if (0)//(input[0][0] == output[0][0])
        {
            std::cerr << "Error time range of channel 1 and channel 2 are not identical" << std::endl;
            std::cout << "Channel 1: " << input[0][i] << " Channel 2: " << output[0][0] << std::endl;
            continue;
        }

        for (int i2 = 0; i2 < points; i2++) {
            stream << input[0][i2] * 1000 << "," << input[1][i2] << "," << output[1][i2] << std::endl;
        }
        delete output[0];
        delete output[1];

        delete input[0];
        delete input[1];

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
    if (!wg || !o) {
        std::cout << __LINE__ << ": Error invalid arguments" << std::endl;
        return false;
    }

    double voltage = i_voltage / 10.0;
    time_t t = time(nullptr);
    auto ret = wg->setOffset(2 * voltage);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting the frequency " << ret << ")"
                  << std::endl;
        return false;
    }

    std::cout << "Set frequency " << frequency << std::endl;
    ret = wg->setFrequency(frequency);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting the channel_range " << ret << ")"
                  << std::endl;
        return false;
    }

    std::string file_name = DATA_PATH + std::string("/") + std::to_string(frequency) + "/" + std::to_string(CHIP)
                            + "_" + std::to_string(cell)
                            + "_" + std::to_string(frequency)
                            + "_" + std::to_string(i_voltage)
                            + "_" + std::to_string(t);

    std::cout << "Write data to: " << file_name << std::endl;

    if (!capture_data(file_name, o, frequency)) {
        std::cout << __LINE__ << ": Error while capturing data " << ret << ")"
                  << std::endl;
        return false;
    }

    return true;
    // MetaFileHandler metaFileHandler(file_name.c_str(), DATA_PATH);  // TODO: FIX
    // return metaFileHandler.save_meta(frequency, voltage, cell);
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
    if (!wg || !o) {
        std::cout << __LINE__ << ": Error invalid arguments" << std::endl;
        return false;
    }

    wg->setFunction(KST33500::SIN);
    wg->setOffset(0);
    wg->setOffset(1 * 2);

    PIL_ERROR_CODE ret;
    for (int i = 2; i <= MAX_VOLTAGE; i++) {
        if (!wg->isOutputEnabled()) {
            ret = wg->turnOn();
            if (ret) {
                std::cout << "Error could not enable output (" << ret << ")" << std::endl;
                return false;
            }
        }
        printf("Set voltage to : %.2f V", (double) i / 10.0);
        printf("\r");
        fflush(stdout);
        if (!set_params(wg, o, 100, i)) {
            std::cout << __LINE__ << ": set_params retuns an error" << std::endl;
            return false;
        }

        sleepSecs(0.5);
    }

    for (int i3 = 0; i3 < NR_FREQUENCIES; i3++) {
        for (int v = MIN_VOLTAGE; v <= MAX_VOLTAGE; v++) {
            int f = frequencies[i3];
            std::cout << "Start test execution ..." << std::endl;
            sleepSecs(3.0);
            if (!set_params(wg, o, f, v)) {
                std::cout << __LINE__ << ": set_params retuns an error" << std::endl;
                return false;
            }

            if (!single_test(cell, wg, o, f, v)) {
                std::cout << __LINE__ << ": single_test returns an error" << std::endl;
                // return false;
            }
        }
    }
    ret = wg->turnOff();
    if (ret) {
        std::cout << "Error while disabling the function generator output" << std::endl;
        return false;
    }
    return true;
}


bool config(KST3000 *o) {
    if (!o) {
        std::cout << __LINE__ << ": Error invalid arguments" << std::endl;
        return false;
    }

    std::string mode = "NORmal";
    auto ret = o->setWaveformPointsMode(mode);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting waveform points mode " << ret << ")"
                  << std::endl;
        return false;
    }

    ret = o->setWaveformPoints(1000);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting waveform points " << ret << ")"
                  << std::endl;
        return false;
    }

    return true;
}

void testPulses(std::string &args) {
    auto argumentList = CommandLineInterface::splitArguments(args);
    if (argumentList.empty()) {
        std::cout << "Error: argument list is empty" << std::endl;
        return;
    }

    auto *wg = new KST33500(WG_IP, TIMEOUT);

    auto *o = new KST3000(O_IP, TIMEOUT);

    try {
        int cellID = std::stoi(argumentList[0]);

        if (!connect_wave_generator(wg)) {
            std::cout << "Unable to connect to waveform generator " << std::endl;
            return;
        }
        if (!connect_oscilloscope(o)) {
            std::cout << "Unable to connect to the oscilloscope" << std::endl;
            return;
        }
        if (!config(o))
            return;

        printf("Measure cell %d\n", cellID);
        if (!memristor_frequency_test(cellID, wg, o))
            return;
    } catch (std::invalid_argument &e) {
        std::cout << "    Error argument is not a valid cell ID " << std::endl;
    }
    delete wg;
    delete o;

    // TODO check
}

enum ResistiveState {
    HIGH_RESISTIVE_STATE,
    LOW_RESISTIVE_STATE
};

bool changeResistiveState(KST33500 *wg, ResistiveState resistiveState) {
/*    if(!wg->setFunction(KST33500::SIN))
    {
        std::cout << "Error setting function SIN not possible" << std::endl;
        return false;
    }*/

    auto ret = wg->setPhase(resistiveState == HIGH_RESISTIVE_STATE ? 180 : 0);
    if (ret != PIL_NO_ERROR) {
        std::cout << "Unable to set phase" << std::endl;
        return false;
    }
/*
    if(!wg->frequency(0.25)){
        std::cout << "Error could not set frequency" << std::endl;
        return false;
    }

    if(!wg->voltage(1.2))
    {
        std::cout << "Could not set voltage" << std::endl;
        return false;
    }*/
    sleepSecs(0);
    ret = wg->turnOn();
    if (ret != PIL_NO_ERROR) {
        std::cout << "Could not enable the output" << std::endl;
        return false;
    }
    return true;
}

void measureThresholds(std::string &args) {


    auto *wg = new KST33500(WG_IP, 10000);

    if (!connect_wave_generator(wg)) {
        std::cout << "Unable to connect to waveform generator " << std::endl;
        return;
    }
    auto *oscilloscope = new KST3000(O_IP, TIMEOUT);
    if (!connect_oscilloscope(oscilloscope)) {
        std::cout << "Unable to connect to oscilloscope " << std::endl;
        return;
    }

    std::string mode = "NORmal";
    auto ret = oscilloscope->setWaveformPointsMode(mode);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting waveform points mode " << ret << ")"
                  << std::endl;
        return;
    }

    ret = oscilloscope->setWaveformPoints(1000);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting waveform points " << ret << ")"
                  << std::endl;
        return;
    }

    ret = oscilloscope->setChannelRange(Oscilloscope::CHANNEL_1, 1.2 * 2, Oscilloscope::VOLT);
    if (ret != PIL_NO_ERROR) // *2 for sin wave
    {
        std::cout << __LINE__ << ": Error while setting the channel_range " << ret << ")"
                  << std::endl;
    }

    ret = oscilloscope->setChannelRange(Oscilloscope::CHANNEL_2, 1.2 * 2, Oscilloscope::VOLT);
    if (ret != PIL_NO_ERROR) // *2 for sin wave
    {
        std::cout << __LINE__ << ": Error while setting the channel_range " << ret << ")" << std::endl;
    }


    ret = oscilloscope->setTimeRange(10);
    if (ret != PIL_NO_ERROR) // collect 1 cycle every time
    {
        std::cout << __LINE__ << ": Error while setting the time_range " << ret << ")" << std::endl;
        //return false;
    }

    std::thread th([oscilloscope]() {
        std::stringstream stream;
        stream << "time" << "," << "input" << "," << "output" << std::endl;

        time_t t = time(nullptr);
        std::string file_name =
                DATA_PATH + std::string("/measure_threshold_") + std::to_string(CHIP) + "_" +
                std::to_string(1) // TODO
                + "_" + std::to_string(25) + "_" + std::to_string(1.2) + "_" + std::to_string(t);

        int points;
        oscilloscope->getWaveformPoints(&points);
        if (points == 0) {
            std::cerr << "Available points is 0 Return from function" << std::endl;
        }

        oscilloscope->digitize(Oscilloscope::CHANNEL_1);
        oscilloscope->setWaveformSource(Oscilloscope::CHANNEL_1);
        double *input[2];
        input[0] = new double[points];
        input[1] = new double[points];
        int ch1Ret = oscilloscope->getRealData(input);

        oscilloscope->digitize(Oscilloscope::CHANNEL_2);
        oscilloscope->setWaveformSource(Oscilloscope::CHANNEL_2);
        double *output[2];
        output[0] = new double[points];
        output[1] = new double[points];
        int ch2Ret = oscilloscope->getRealData(output);


        for (int i2 = 0; i2 < points; i2++) {
            stream << input[0][i2] * 1000 << "," << input[1][i2] << "," << output[1][i2]
                   << std::endl;
        }
        delete output[0];
        delete output[1];

        delete input[0];
        delete input[1];

        write_to_file(stream.str(), file_name);
    });



    // First set cell to low resistive state, otherwise the threshold could not be determined
    // Device is already in low resistive state
    if (!changeResistiveState(wg, HIGH_RESISTIVE_STATE)) {
        std::cout << "Exit test execution" << std::endl;
        return;
    }

    sleepSecs(2);
    ret = wg->turnOff();
    if (ret != PIL_NO_ERROR) {
        std::cout << "Disable output failed" << std::endl;
    }

    sleepSecs(4);

    if (!changeResistiveState(wg, LOW_RESISTIVE_STATE)) {
        std::cout << "Exit test execution" << std::endl;
        return;
    }

    sleepSecs(2);
    ret = wg->turnOff();
    if (ret != PIL_NO_ERROR) {
        std::cout << "Disable output" << std::endl;
    }
    th.join();

}

void capture_osci(std::string &args) {
    auto argumentList = CommandLineInterface::splitArguments(args);
    auto *oscilloscope = new KST3000(O_IP, TIMEOUT);
    if (!connect_oscilloscope(oscilloscope)) {
        std::cout << "Unable to connect to oscilloscope " << std::endl;
        return;
    }

    std::string mode = "NORmal";
    auto ret = oscilloscope->setWaveformPointsMode(mode);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting waveform points mode " << ret << ")" << std::endl;
        return;
    }

    ret = oscilloscope->setWaveformPoints(1000);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting waveform points " << ret << ")" << std::endl;
        return;
    }

    //   if(!oscilloscope->set_channel_range(1.2 * 2, 1)) // *2 for sin wave
    {
        //   std::cout << __LINE__ << ": Error while setting the channel_range " << oscilloscope->return_error_message() << ")"
        //           << std::endl;
    }

    // if(!oscilloscope->set_channel_range(1.2 * 2, 2)) // *2 for sin wave
    {
        //std::cout << __LINE__ << ": Error while setting the channel_range " << oscilloscope->return_error_message() << ")" << std::endl;
    }


    //if(!oscilloscope->set_time_range(10)) // collect 1 cycle every time
    {
        // std::cout << __LINE__ << ": Error while setting the time_range " << oscilloscope->return_error_message() << ")" << std::endl;
        //return false;
    }

    oscilloscope->setWaveformSource(Oscilloscope::CHANNEL_1);

    int points;
    oscilloscope->getWaveformPoints(&points);
    double *input[2];
    input[0] = new double[points];
    input[1] = new double[points];
    oscilloscope->getRealData(input);

    std::stringstream stream;
    stream << "time" << "," << "value" << std::endl;
    for (int i2 = 0; i2 < points; i2++) {
        stream << input[0][i2] * 1000 << "," << input[1][i2] << std::endl;
    }

    delete input[0];
    delete input[1];


    write_to_file(stream.str(), "./Ausgabe.csv");
}

void testCellAllFrequencies(std::string &args) {
    auto argumentList = CommandLineInterface::splitArguments(args);
    if (argumentList.empty()) {
        std::cout << "Error: argument list is empty" << std::endl;
        return;
    }

    auto *wg = new KST33500(WG_IP, TIMEOUT);

    auto *o = new KST3000(O_IP, TIMEOUT);

    try {
        int cellID = std::stoi(argumentList[0]);

        if (!connect_wave_generator(wg)) {
            std::cout << "Unable to connect to waveform generator " << std::endl;
            return;
        }
        if (!connect_oscilloscope(o)) {
            std::cout << "Unable to connect to the oscilloscope" << std::endl;
            return;
        }
        if (!config(o))
            return;

        printf("Measure cell %d\n", cellID);
        if (!memristor_frequency_test(cellID, wg, o))
            return;
    } catch (std::invalid_argument &e) {
        std::cout << "    Error argument is not a valid cell ID " << std::endl;
    }
    delete wg;
    delete o;
}

void capture_osci_2(std::string &args) {
    auto argumentList = CommandLineInterface::splitArguments(args);
    auto *oscilloscope = new KST3000(O_IP, TIMEOUT);
    if (!connect_oscilloscope(oscilloscope)) {
        std::cout << "Unable to connect to oscilloscope " << std::endl;
        return;
    }

    std::string mode = "NORmal";
    auto ret = oscilloscope->setWaveformPointsMode(mode);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting waveform points mode " << ret << ")"
                  << std::endl;
        return;
    }

    ret = oscilloscope->setWaveformPoints(1000);
    if (ret != PIL_NO_ERROR) {
        std::cout << __LINE__ << ": Error while setting waveform points " << ret << ")"
                  << std::endl;
        return;
    }

    //   if(!oscilloscope->set_channel_range(1.2 * 2, 1)) // *2 for sin wave
    {
        //   std::cout << __LINE__ << ": Error while setting the channel_range " << oscilloscope->return_error_message() << ")"
        //           << std::endl;
    }

    // if(!oscilloscope->set_channel_range(1.2 * 2, 2)) // *2 for sin wave
    {
        //std::cout << __LINE__ << ": Error while setting the channel_range " << oscilloscope->return_error_message() << ")" << std::endl;
    }


    //if(!oscilloscope->set_time_range(10)) // collect 1 cycle every time
    {
        // std::cout << __LINE__ << ": Error while setting the time_range " << oscilloscope->return_error_message() << ")" << std::endl;
        //return false;
    }

    oscilloscope->setWaveformSource(Oscilloscope::CHANNEL_1);

    int points;
    oscilloscope->getWaveformPoints(&points);
    double *input[2];
    input[0] = new double[points];
    input[1] = new double[points];
    oscilloscope->getRealData(input);

    oscilloscope->setWaveformSource(Oscilloscope::CHANNEL_2);

    double *input2[2];
    input2[0] = new double[points];
    input2[1] = new double[points];
    oscilloscope->getRealData(input2);

    std::stringstream stream;
    stream << "time" << "," << "value" << "," << "value2" << std::endl;
    for (int i2 = 0; i2 < points; i2++) {
        stream << input[0][i2] * 1000 << "," << input[1][i2] << "," << input2[1][i2] << std::endl;
    }

    delete input[0];
    delete input[1];


    write_to_file(stream.str(), "./Ausgabe.csv");
}

int main() {

    CommandLineInterface cli;
    CommandLineInterface::addCustomCommandLineOption("t_a",
                                                       "- Starts a memristor hysterese loop test with different freqnencies",
                                                       testCellAllFrequencies);


    CommandLineInterface::addCustomCommandLineOption("testPulses",
                                                       "- Starts a memristor hysterese loop test with different freqnencies",
                                                       testPulses);


    CommandLineInterface::addCustomCommandLineOption("m_t",
                                                       "Measures the voltage after which the resistance changes as a function",
                                                       measureThresholds);

    CommandLineInterface::addCustomCommandLineOption("capture_current_osci", "Capture the current oscilloscope data.",
                                                       capture_osci);

    CommandLineInterface::addCustomCommandLineOption("a", "Capture the current oscilloscope data.",
                                                       capture_osci_2);
    cli.Start();

    return 0;
}
