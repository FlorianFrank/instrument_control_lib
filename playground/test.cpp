//
// Created by sueaki on 09.06.22.
//

#include "test.h"
#include "unistd.h"
#include "ctlib/Logging.hpp"
#include "devices/KST3000.h"

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

int testOscillosope() {
    auto osci = KST3000("1282", 0, nullptr);
    osci.setTriggerEdge(Oscilloscope::POS_EDGE);
    return 0;
}

bool doubleInRange(double value, double comparisonValue, double tolerance) {
    return value >= comparisonValue - tolerance && value <= comparisonValue + tolerance;
}

int testSMU(string ip, double doubleTolerance) {
    PIL::Logging logger(INFO_LVL, nullptr);
    KEI2600 *smu = new KEI2600(ip.c_str(), 0, &logger);
    if(smu->Connect() != PIL_NO_ERROR) {
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
    smu->Exec(print_str, nullptr, buffer);
    cout << "bitxor(10, 9) is: " << buffer;


    // beep
    smu->enableBeep(false);
    //smu->beep(100, 1000, false);
    //usleep(1000000);
    smu->disableBeep();

    smu->enableSourceAutoRange(SMU::VOLTAGE, SMU::CHANNEL_A);
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 0.5, false);
    smu->enableMeasureAutoRange(SMU::VOLTAGE, SMU::CHANNEL_A, false);
    smu->turnOn(SMU::CHANNEL_A, false);
    double measureV;
    smu->measure(SMU::VOLTAGE, SMU::CHANNEL_A, &measureV, false);
    if (!doubleInRange(measureV, 0.5, doubleTolerance)) {
        cout << "ERROR! Voltage test failed!\n";
        exit(1);
    } else {
        cout << "Voltage test passed!\n";
        smu->disableSourceAutoRange(SMU::VOLTAGE, SMU::CHANNEL_A);
        smu->disableMeasureAutoRange(SMU::VOLTAGE, SMU::CHANNEL_A, false);
        smu->turnOff(SMU::CHANNEL_A, false);
    }

    smu->setSourceRange(SMU::VOLTAGE, SMU::CHANNEL_B, 0.5);
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_B, 0.5, false);
    smu->setMeasureRange(SMU::VOLTAGE, SMU::CHANNEL_B, 0.5);
    smu->turnOn(SMU::CHANNEL_B, false);
    smu->measure(SMU::VOLTAGE, SMU::CHANNEL_B, &measureV, false);
    if (!doubleInRange(measureV, 0.5, doubleTolerance)) {
        cout << "ERROR! Voltage on Channel b test failed!\n";
        exit(1);
    } else {
        cout << "Voltage test on Channel b passed!\n";
        smu->enableSourceAutoRange(SMU::VOLTAGE, SMU::CHANNEL_B);
        smu->enableMeasureAutoRange(SMU::VOLTAGE, SMU::CHANNEL_B, false);
        smu->turnOff(SMU::CHANNEL_B, false);
    }

    smu->enableSourceAutoRange(SMU::CURRENT, SMU::CHANNEL_B);
    smu->setLevel(SMU::CURRENT, SMU::CHANNEL_B, -1.1, false);
    smu->enableMeasureAutoRange(SMU::CURRENT, SMU::CHANNEL_B, false);
    smu->turnOn(SMU::CHANNEL_B, false);
    double measureI;
    smu->measure(SMU::CURRENT, SMU::CHANNEL_B, &measureI, false);
    if (measureI != -1.1) {
        cout << "ERROR! Current test failed!\n";
        exit(1);
    } else {
        cout << "Current test passed!\n";
        smu->disableSourceAutoRange(SMU::CURRENT, SMU::CHANNEL_B);
        smu->disableMeasureAutoRange(SMU::CURRENT, SMU::CHANNEL_B, false);
        smu->turnOn(SMU::CHANNEL_B, false);
    }

    smu->setSourceRange(SMU::CURRENT, SMU::CHANNEL_B, -1.1);
    smu->setLevel(SMU::CURRENT, SMU::CHANNEL_B, -1.1, false);
    smu->setMeasureRange(SMU::CURRENT, SMU::CHANNEL_B, -1.1);
    smu->turnOn(SMU::CHANNEL_B, false);
    smu->measure(SMU::CURRENT, SMU::CHANNEL_B, &measureI, false);
    if (measureI != -1.1) {
        cout << "ERROR! Current test on Channel b failed!\n";
        exit(1);
    } else {
        cout << "Current test on Channel b passed!\n";
        smu->disableSourceAutoRange(SMU::CURRENT, SMU::CHANNEL_B);
        smu->disableMeasureAutoRange(SMU::CURRENT, SMU::CHANNEL_B, false);
        smu->turnOn(SMU::CHANNEL_B, false);
    }

    smu->setLimit(SMU::CURRENT, SMU::CHANNEL_B, -1.5, false);
    // this should fail.
    smu->setLevel(SMU::CURRENT, SMU::CHANNEL_B, -1.6, false);
    cout << "Current on Channel a should not be -1.6. Maybe 0.5\n";
    sleep(2);

    smu->setLimit(SMU::VOLTAGE, SMU::CHANNEL_B, 0.5, false);
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_B, 0.6, false);
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

