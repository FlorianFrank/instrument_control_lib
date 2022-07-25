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

    power->setCurrent(DCPowerSupply::CHANNEL_1, 0.15);
    double measureI;
    power->getCurrent(DCPowerSupply::CHANNEL_1, &measureI);
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
    if(!connectRet) {
        cout << smu->ReturnErrorMessage() << std::endl;
        return 1;
    }

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

    smu->enableSourceAutoRange(SMU::VOLTAGE, SMU::CHANNEL_A);
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 0.5);
    smu->enableMeasureAutoRange(SMU::VOLTAGE, SMU::CHANNEL_A);
    smu->turnOn(SMU::CHANNEL_A);
    double measureV;
    smu->measure(SMU::VOLTAGE, SMU::CHANNEL_A, &measureV);
    if (measureV != 0.5) {
        cout << "ERROR! Voltage test failed!\n";
        exit(1);
    } else {
        cout << "Voltage test passed!\n";
        smu->disableSourceAutoRange(SMU::VOLTAGE, SMU::CHANNEL_A);
        smu->disableMeasureAutoRange(SMU::VOLTAGE, SMU::CHANNEL_A);
        smu->turnOff(SMU::CHANNEL_A);
    }

    smu->setSourceRange(SMU::VOLTAGE, SMU::CHANNEL_B, 0.5);
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_B, 0.5);
    smu->setMeasureRange(SMU::VOLTAGE, SMU::CHANNEL_B, 0.5);
    smu->turnOn(SMU::CHANNEL_B);
    smu->measure(SMU::VOLTAGE, SMU::CHANNEL_B, &measureV);
    if (measureV != 0.5) {
        cout << "ERROR! Voltage on Channel b test failed!\n";
        exit(1);
    } else {
        cout << "Voltage test on Channel b passed!\n";
        smu->enableSourceAutoRange(SMU::VOLTAGE, SMU::CHANNEL_B);
        smu->enableMeasureAutoRange(SMU::VOLTAGE, SMU::CHANNEL_B);
        smu->turnOff(SMU::CHANNEL_B);
    }

    smu->enableSourceAutoRange(SMU::CURRENT, SMU::CHANNEL_B);
    smu->setLevel(SMU::CURRENT, SMU::CHANNEL_B, -1.1);
    smu->enableMeasureAutoRange(SMU::CURRENT, SMU::CHANNEL_B);
    smu->turnOn(SMU::CHANNEL_B);
    double measureI;
    smu->measure(SMU::CURRENT, SMU::CHANNEL_B, &measureI);
    if (measureI != -1.1) {
        cout << "ERROR! Current test failed!\n";
        exit(1);
    } else {
        cout << "Current test passed!\n";
        smu->disableSourceAutoRange(SMU::CURRENT, SMU::CHANNEL_B);
        smu->disableMeasureAutoRange(SMU::CURRENT, SMU::CHANNEL_B);
        smu->turnOn(SMU::CHANNEL_B);
    }

    smu->setSourceRange(SMU::CURRENT, SMU::CHANNEL_B, -1.1);
    smu->setLevel(SMU::CURRENT, SMU::CHANNEL_B, -1.1);
    smu->setMeasureRange(SMU::CURRENT, SMU::CHANNEL_B, -1.1);
    smu->turnOn(SMU::CHANNEL_B);
    smu->measure(SMU::CURRENT, SMU::CHANNEL_B, &measureI);
    if (measureI != -1.1) {
        cout << "ERROR! Current test on Channel b failed!\n";
        exit(1);
    } else {
        cout << "Current test on Channel b passed!\n";
        smu->disableSourceAutoRange(SMU::CURRENT, SMU::CHANNEL_B);
        smu->disableMeasureAutoRange(SMU::CURRENT, SMU::CHANNEL_B);
        smu->turnOn(SMU::CHANNEL_B);
    }

    smu->setLimit(SMU::CURRENT, SMU::CHANNEL_B, -1.5);
    // this should fail.
    smu->setLevel(SMU::CURRENT, SMU::CHANNEL_B, -1.6);
    cout << "Current on Channel a should not be -1.6. Maybe 0.5\n";
    sleep(2);

    smu->setLimit(SMU::VOLTAGE, SMU::CHANNEL_B, 0.5);
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_B, 0.6);
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

