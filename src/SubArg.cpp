//
// Created by florianfrank on 15.08.22.
//

#include "SubArg.h"

SubArg::SubArg(std::string arg, std::string prefix, std::string postfix)
{
    AddElem(arg, prefix, postfix);
}

SubArg& SubArg::AddElem(std::string value, std::string prefix, std::string postfix)
{
    std::tuple<std::string, std::string, std::string> tuple = {prefix, value, postfix};
    m_SubArgElem.push_back(tuple);
    return *this;
}

SubArg& SubArg::AddElem(int value, std::string prefix, std::string postfix)
{
    std::tuple<std::string, std::string, std::string> tuple = {prefix, std::to_string(value), postfix};
    m_SubArgElem.push_back(tuple);
    return *this;
}

SubArg& SubArg::AddElem(double value, std::string prefix, std::string postfix)
{
    std::tuple<std::string, std::string, std::string> tuple = {prefix, std::to_string(value), postfix};
    m_SubArgElem.push_back(tuple);
    return *this;
}

std::string SubArg::toString()
{
    std::string retStr;
    for (auto elem : m_SubArgElem)
    {
        retStr += std::get<0>(elem) + std::get<1>(elem) + std::get<2>(elem);
    }
    return retStr;
}