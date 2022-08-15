//
// Created by florianfrank on 13.08.22.
//
#include "ExecArgs.h"


ExecArgs & ExecArgs::AddArgument(std::string argument, int value, std::string seperator)
{
    auto value_tuple = std::make_tuple(argument, std::to_string(value));
    auto arg_tuple = std::make_tuple(ARG_TYPE_INTEGER, seperator, value_tuple);
    m_ArgList.push_back(arg_tuple);
    return *this;
}

ExecArgs & ExecArgs::AddArgument(std::string argument, double value, std::string seperator)
{
    auto value_tuple = std::make_tuple(argument, std::to_string(value));
    auto arg_tuple = std::make_tuple(ARG_TYPE_FLOAT, seperator, value_tuple);
    m_ArgList.push_back(arg_tuple);
    return *this;
}

ExecArgs & ExecArgs::AddArgument(std::string argument, std::string value, std::string seperator)
{
    auto value_tuple = std::make_tuple(argument, value);
    auto arg_tuple = std::make_tuple(ARG_TYPE_STRING, seperator, value_tuple);
    m_ArgList.push_back(arg_tuple);
    return *this;
}

std::string ExecArgs::GetArgumentsAsString()
{
    std::string retStr;
    for (auto arg: m_ArgList)
    {
        retStr += GetArgumentFromArgTuple(arg) +
                GetSeperatorFromArgTuple(arg) + GetValueFromArgTuple(arg) + "";
    }
    return retStr;
}

ExecArgs::ARGUMENT_TYPE ExecArgs::GetArgumentTypeFromArgTuple(ExecArgs::ArgTuple &argTuple)
{
    return std::get<0>(argTuple);
}

std::string ExecArgs::GetSeperatorFromArgTuple(ExecArgs::ArgTuple &argTuple)
{
    return std::get<1>(argTuple);
}
#include <iostream>
std::string ExecArgs::GetArgumentFromArgTuple(ExecArgs::ArgTuple &argTuple)
{
    std::tuple<std::string,std::string> keyValuePair = std::get<2>(argTuple);
    return std::get<0>(keyValuePair);
}

std::string ExecArgs::GetValueFromArgTuple(ExecArgs::ArgTuple &argTuple)
{
    auto keyValuePair = std::get<2>(argTuple);
    return std::get<1>(keyValuePair);
}

ExecArgs & ExecArgs::AddArgument(SubArg argument, int value, std::string seperator)
{
    return AddArgument(argument.toString(), value, seperator);
}

ExecArgs & ExecArgs::AddArgument(SubArg argument, double value, std::string seperator)
{
    return AddArgument(argument.toString(), value, seperator);
}

ExecArgs & ExecArgs::AddArgument(SubArg argument, std::string value, std::string seperator)
{
    return AddArgument(argument.toString(), value, seperator);
}

ExecArgs &ExecArgs::AddArgument(SubArg argument, SubArg& value, std::string seperator)
{
    return AddArgument(argument.toString(), value.toString(), seperator);;
}
