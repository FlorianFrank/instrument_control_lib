//
// Created by florianfrank on 06.09.21.
//


#include "CommandLineInterface.h"
#include "ctlib/Logging.hpp"

int main()
{
    PIL::Logging logger(PIL::DEBUG, nullptr);
    CommandLineInterface commandLineInterface(&logger);
    commandLineInterface.Start();

    return 0;
}