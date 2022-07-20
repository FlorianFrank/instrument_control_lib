//
// Created by sueaki on 09.06.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_TEST_H
#define INSTRUMENT_CONTROL_LIB_TEST_H

#include "iostream"
#include "devices/KEI2600.h"
#include "devices/SPD1305.h"

using namespace std;

int testSMU(string ip);

int testSPD();

#endif //INSTRUMENT_CONTROL_LIB_TEST_H
