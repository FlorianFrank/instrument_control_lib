//
// Created by sueaki on 09.06.22.
//

#include "test.h"
#include "unistd.h"
#include "ctlib/Logging.hpp"

using namespace SMU;

int testSPD() {
//    SPD1305 power("132.231.14.176", 0);
    SPD1305 *power = new SPD1305("132.231.14.176", 0);
    power->Connect();

    power->set_current(0.15);
    double measureI = power->get_current();
    if (measureI != 0.15) {
        cout << "ERROR! Current test failed.\n";
        exit(1);
    } else {
        cout << "Current test passed.\n";
    }
    power->Disconnect();
    return 0;
}

int testSMU(string ip) {
    PIL::Logging logger(INFO_LVL, nullptr);
    KEI2600 *smu = new KEI2600(&logger, ip.c_str(), 0);
    bool connectRet = smu->Connect();
    if(!connectRet)
        cout << smu->ReturnErrorMessage() << std::endl;

    string id = smu->GetDeviceIdentifier();
    cout << "Connected to: " + id << endl;


    string bo_str = "testResult = bit.bitxor(10, 9)";
    // check print() function of TSP
    string print_str = "print(testResult)";
    smu->Exec(bo_str);
    char buffer[1024] = "0";
    smu->Exec(print_str, buffer);
    cout << "bitxor(10, 9) is: " << buffer;


    // beep
    smu->enableBeep();
    smu->beep();
    usleep(1000000);
    smu->disableBeep();

    smu->enableSourceAutoRange(VOLTAGE, CHANNEL_A);
    smu->setLevel(VOLTAGE, CHANNEL_A, 0.5);
    smu->enableMeasureAutoRange(VOLTAGE, CHANNEL_A);
    smu->turnOn(CHANNEL_A);
    double measureV = smu->readV(CHANNEL_A);
    if (measureV != 0.5) {
        cout << "ERROR! Voltage test failed!\n";
        exit(1);
    } else {
        cout << "Voltage test passed!\n";
        smu->disableSourceAutoRange(VOLTAGE, CHANNEL_A);
        smu->disableMeasureAutoRange(VOLTAGE, CHANNEL_A);
        smu->turnOff(CHANNEL_A);
    }

    smu->setSourceRange(VOLTAGE, CHANNEL_B, 0.5);
    smu->setLevel(VOLTAGE, CHANNEL_B, 0.5);
    smu->setMeasureRange(VOLTAGE, CHANNEL_B, 0.5);
    smu->turnOn(CHANNEL_B);
    measureV = smu->readV(CHANNEL_B);
    if (measureV != 0.5) {
        cout << "ERROR! Voltage on Channel b test failed!\n";
        exit(1);
    } else {
        cout << "Voltage test on Channel b passed!\n";
        smu->enableSourceAutoRange(VOLTAGE, CHANNEL_B);
        smu->enableMeasureAutoRange(VOLTAGE, CHANNEL_B);
        smu->turnOff(CHANNEL_B);
    }

    smu->enableSourceAutoRange(CURRENT, CHANNEL_B);
    smu->setLevel(CURRENT, CHANNEL_B, -1.1);
    smu->enableMeasureAutoRange(CURRENT, CHANNEL_B);
    smu->turnOn(CHANNEL_B);
    double measureI = smu->readI(CHANNEL_B);
    if (measureI != -1.1) {
        cout << "ERROR! Current test failed!\n";
        exit(1);
    } else {
        cout << "Current test passed!\n";
        smu->disableSourceAutoRange(CURRENT, CHANNEL_B);
        smu->disableMeasureAutoRange(CURRENT, CHANNEL_B);
        smu->turnOn(CHANNEL_B);
    }

    smu->setSourceRange(CURRENT, CHANNEL_B, -1.1);
    smu->setLevel(CURRENT, CHANNEL_B, -1.1);
    smu->setMeasureRange(CURRENT, CHANNEL_B, -1.1);
    smu->turnOn(CHANNEL_B);
    measureI = smu->readI( CHANNEL_B);
    if (measureI != -1.1) {
        cout << "ERROR! Current test on Channel b failed!\n";
        exit(1);
    } else {
        cout << "Current test on Channel b passed!\n";
        smu->disableSourceAutoRange(CURRENT, CHANNEL_B);
        smu->disableMeasureAutoRange(CURRENT, CHANNEL_B);
        smu->turnOn(CHANNEL_B);
    }

    smu->setLimit(CURRENT, CHANNEL_B, -1.5);
    // this should fail.
    smu->setLevel(CURRENT, CHANNEL_B, -1.6);
    cout << "Current on Channel a should not be -1.6. Maybe 0.5\n";
    sleep(2);

    smu->setLimit(VOLTAGE, CHANNEL_B, 0.5);
    smu->setLevel(VOLTAGE, CHANNEL_B, 0.6);
    cout << "Voltage on Channel a should not be 0.6. Maybe 1.5\n";
    sleep(2);

    cout << "Input whatever TSP command to execute:(Ctrl + D quit)\n";
    string command;
    while (getline(cin, command)) {
        smu->Exec(command);
    }
    smu->Disconnect();
    
    return 0;
}

