/**
 * @brief This file contains the implementation of sub-arguments. E.g. when a command has the structure
 * smua.voltage = 3 then smua.voltage can be encoded within a SubArg.
 * @author Florian Frank
 */
#include "SubArg.h"

#include <utility>

/**
 * @brief Constructor initializes the first part of a sub-argument.
 * E.g. arg: smua, prefix: ., suffix: =, results in ".smu="
 * @param arg argument which is enclosed by a prefix and suffix.
 * @param prefix is directly placed in front of arg.
 * @param suffix is placed after arg.
 */
SubArg::SubArg(const std::string& arg, const std::string& prefix, const std::string& suffix)
{
    AddElem(arg, prefix, suffix);
}

/**
 * @brief Add a new string element to the SubArg-object.
 * It is directly concatenated to the previous arguments.
 * @param arg string argument which is added enclosed by a prefix and suffix.
 * @param prefix is directly placed in front of arg.
 * @param suffix is placed after arg.
 * @return pointer to current object to allow a chain of arguments, which can be written in a compact manner.
 */
SubArg& SubArg::AddElem(const std::string& arg, const std::string& prefix, const std::string& suffix)
{
    std::tuple<std::string, std::string, std::string> tuple = {prefix, arg, suffix};
    m_SubArgElem.push_back(tuple);
    return *this;
}

/**
 * @brief Add a new integer element to the SubArg-object.
 * It is directly concatenated to the previous arguments.
 * @param arg string argument which is added enclosed by a prefix and suffix.
 * @param prefix is directly placed in front of arg.
 * @param suffix is placed after arg.
 * @return pointer to current object to allow a chain of arguments, which can be written in a compact manner.
 */
SubArg& SubArg::AddElem(int value, const std::string& prefix, const std::string& postfix)
{
    std::tuple<std::string, std::string, std::string> tuple = {prefix, std::to_string(value), postfix};
    m_SubArgElem.push_back(tuple);
    return *this;
}

/**
 * @brief Add a new double element to the SubArg-object.
 * It is directly concatenated to the previous arguments.
 * @param arg string argument which is added enclosed by a prefix and suffix.
 * @param prefix is directly placed in front of arg.
 * @param suffix is placed after arg.
 * @return pointer to current object to allow a chain of arguments, which can be written in a compact manner.
 */
SubArg& SubArg::AddElem(double value, const std::string& prefix, const std::string& postfix)
{
    std::tuple<std::string, std::string, std::string> tuple = {prefix, std::to_string(value), postfix};
    m_SubArgElem.push_back(tuple);
    return *this;
}

/**
 * @brief Transform all arguments to a string to be passed to a ExecArgs object.
 * @return string representation of all subarguments.
 */
std::string SubArg::toString()
{
    // TODO return local variable!
    std::string retStr;
    for (auto elem : m_SubArgElem)
    {
        retStr += std::get<0>(elem) + std::get<1>(elem) + std::get<2>(elem);
    }
    return retStr;
}