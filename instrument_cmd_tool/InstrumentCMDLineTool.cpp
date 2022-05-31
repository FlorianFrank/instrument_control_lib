//
// Created by florianfrank on 06.09.21.
//


#include "CommandLineInterface.h"
#include "ctlib/Logging.hpp"

int main()
{
    PIL::Logging logger(INFO_LVL, nullptr);
    CommandLineInterface commandLineInterface(&logger);
    commandLineInterface.start();

    return 0;
}