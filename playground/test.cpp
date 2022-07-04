//
// Created by sueaki on 09.06.22.
//

#include "test.h"
#include "unistd.h"
#include "ctlib/Logging.hpp"

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

    smu->enableSourceAutorangeV();
    smu->setLevelV(0.5);
    smu->enableMeasureAutorangeV();
    smu->turnOn();
    double measureV = smu->readV();
    if (measureV != 0.5) {
        cout << "ERROR! Voltage test failed!\n";
        exit(1);
    } else {
        cout << "Voltage test passed!\n";
        smu->disableSourceAutorangeV();
        smu->disableMeasureAutorangeV();
        smu->turnOff();
    }

    smu->setSourceRangeV(0.5, 'b');
    smu->setLevelV(0.5, 'b');
    smu->setMeasureRangeV(0.5, 'b');
    smu->turnOn('b');
    measureV = smu->readV('b');
    if (measureV != 0.5) {
        cout << "ERROR! Voltage on Channel b test failed!\n";
        exit(1);
    } else {
        cout << "Voltage test on Channel b passed!\n";
        smu->enableSourceAutorangeV('b');
        smu->enableMeasureAutorangeV('b');
        smu->turnOff();
    }

    smu->enableSourceAutorangeI();
    smu->setLevelI(-1.1);
    smu->enableMeasureAutorangeI();
    smu->turnOn();
    double measureI = smu->readI();
    if (measureI != -1.1) {
        cout << "ERROR! Current test failed!\n";
        exit(1);
    } else {
        cout << "Current test passed!\n";
        smu->disableSourceAutorangeI();
        smu->disableMeasureAutorangeI();
        smu->turnOn();
    }

    smu->setSourceRangeI(-1.1, 'b');
    smu->setLevelI(-1.1, 'b');
    smu->setMeasureRangeI(-1.1, 'b');
    smu->turnOn();
    measureI = smu->readI('b');
    if (measureI != -1.1) {
        cout << "ERROR! Current test on Channel b failed!\n";
        exit(1);
    } else {
        cout << "Current test on Channel b passed!\n";
        smu->disableSourceAutorangeI();
        smu->disableMeasureAutorangeI();
        smu->turnOn();
    }

    smu->setLimitI(-1.5);
    // this should fail.
    smu->setLevelI(-1.6);
    cout << "Current on Channel a should not be -1.6. Maybe 0.5\n";
    sleep(2);

    smu->setLimitV(0.5);
    smu->setLevelV(0.6);
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

