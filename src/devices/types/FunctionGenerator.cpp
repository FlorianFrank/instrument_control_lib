//
// Created by florianfrank on 19.07.22.
//

#include "devices/types/FunctionGenerator.h"

FunctionGenerator::FunctionGenerator(const char *ip, int timeoutInMs, PIL::Logging *logger): Device(ip, timeoutInMs, logger)
{
}

FunctionGenerator::FUNCTION_TYPE FunctionGenerator::getOutputFunction() const
{
    return m_CurrentFunction;
}

bool FunctionGenerator::isOutputEnabled() const
{
    return m_OutputEnabled;
}

double FunctionGenerator::getCurrentAmplitude() const
{
    return m_Amplitude;
}