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

    smu->enableSourceAutoRange(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_A);
    smu->setLevel(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_A, 0.5);
    smu->enableMeasureAutoRange(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_A);
    smu->turnOn(KEI2600::SMU_CHANNEL_A);
    double measureV = smu->readV(KEI2600::SMU_CHANNEL_A);
    if (measureV != 0.5) {
        cout << "ERROR! Voltage test failed!\n";
        exit(1);
    } else {
        cout << "Voltage test passed!\n";
        smu->disableSourceAutoRange(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_A);
        smu->disableMeasureAutoRange(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_A);
        smu->turnOff(KEI2600::SMU_CHANNEL_A);
    }

    smu->setSourceRange(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_B, 0.5);
    smu->setLevel(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_B, 0.5);
    smu->setMeasureRange(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_B, 0.5);
    smu->turnOn(KEI2600::SMU_CHANNEL_B);
    measureV = smu->readV(KEI2600::SMU_CHANNEL_B);
    if (measureV != 0.5) {
        cout << "ERROR! Voltage on Channel b test failed!\n";
        exit(1);
    } else {
        cout << "Voltage test on Channel b passed!\n";
        smu->enableSourceAutoRange(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_B);
        smu->enableMeasureAutoRange(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_B);
        smu->turnOff(KEI2600::SMU_CHANNEL_B);
    }

    smu->enableSourceAutoRange(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B);
    smu->setLevel(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B, -1.1);
    smu->enableMeasureAutoRange(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B);
    smu->turnOn(KEI2600::SMU_CHANNEL_B);
    double measureI = smu->readI(KEI2600::SMU_CHANNEL_B);
    if (measureI != -1.1) {
        cout << "ERROR! Current test failed!\n";
        exit(1);
    } else {
        cout << "Current test passed!\n";
        smu->disableSourceAutoRange(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B);
        smu->disableMeasureAutoRange(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B);
        smu->turnOn(KEI2600::SMU_CHANNEL_B);
    }

    smu->setSourceRange(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B, -1.1);
    smu->setLevel(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B, -1.1);
    smu->setMeasureRange(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B, -1.1);
    smu->turnOn(KEI2600::SMU_CHANNEL_B);
    measureI = smu->readI( KEI2600::SMU_CHANNEL_B);
    if (measureI != -1.1) {
        cout << "ERROR! Current test on Channel b failed!\n";
        exit(1);
    } else {
        cout << "Current test on Channel b passed!\n";
        smu->disableSourceAutoRange(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B);
        smu->disableMeasureAutoRange(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B);
        smu->turnOn(KEI2600::SMU_CHANNEL_B);
    }

    smu->setLimit(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B, -1.5);
    // this should fail.
    smu->setLevel(KEI2600::CURRENT, KEI2600::SMU_CHANNEL_B, -1.6);
    cout << "Current on Channel a should not be -1.6. Maybe 0.5\n";
    sleep(2);

    smu->setLimit(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_B, 0.5);
    smu->setLevel(KEI2600::VOLTAGE, KEI2600::SMU_CHANNEL_B, 0.6);
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

