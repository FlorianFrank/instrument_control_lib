/**
 * @brief This file contains the implementation of the execution arguments, which should can be passed to the
 * Exec function and can be converted to a string which is sent to the device.
 * @authors Florian Frank
 */
#include "ExecArgs.h"

#include <utility>

/**
 * @brief Add a new argument to the execution argument.
 * e.g. argument = voltage, value = 3 and seperator = : can be transformed into a string: voltage = 3
 * @param argument the argument to which the value gets assigned.
 * @param value an integer value which is internally transformed to a string.
 * @param seperator the seperator which separates the value and argument.
 * @return this object to allow further assignments in one line.
 */
ExecArgs & ExecArgs::AddArgument(std::string argument, int value, std::string seperator)
{
    auto value_tuple = std::make_tuple(argument, std::to_string(value));
    auto arg_tuple = std::make_tuple(ARG_TYPE_INTEGER, seperator, value_tuple);
    m_ArgList.push_back(arg_tuple);
    return *this;
}

/**
 * @brief Add a new argument to the execution argument.
 * e.g. argument = voltage, value = 3 and seperator = : can be transformed into a string: voltage = 3
 * @param argument the argument to which the value gets assigned.
 * @param value a string which is internally transformed to a string.
 * @param seperator the seperator which separates the value and argument.
 * @return this object to allow further assignments in one line.
 */
ExecArgs & ExecArgs::AddArgument(std::string argument, double value, std::string seperator)
{
    auto value_tuple = std::make_tuple(argument, std::to_string(value));
    auto arg_tuple = std::make_tuple(ARG_TYPE_FLOAT, seperator, value_tuple);
    m_ArgList.push_back(arg_tuple);
    return *this;
}

/**
 * @brief Add a new argument to the execution argument.
 * e.g. argument = voltage, value = 3 and seperator = : can be transformed into a string: voltage = 3
 * @param argument the argument to which the value gets assigned.
 * @param value a double value which is internally transformed to a string.
 * @param seperator the seperator which separates the value and argument.
 * @return this object to allow further assignments in one line.
 */
ExecArgs & ExecArgs::AddArgument(const std::string& argument, const std::string& value, const std::string& seperator)
{
    auto value_tuple = std::make_tuple(argument, value);
    auto arg_tuple = std::make_tuple(ARG_TYPE_STRING, seperator, value_tuple);
    m_ArgList.push_back(arg_tuple);
    return *this;
}

/**
 * @brief Add a subarg as argument to the ExecArgList.
 * This is necessary when the argument itself consists of multiple parts. E.g. smua.measurement.voltage = 3.
 * Here smua.measurement.voltage is encapsulated in a SubArg.
 * @param argument sub-argument which internally transformed into a string.
 * @param value an integer value which is internally transformed to a string.
 * @param seperator the seperator which separates the value and argument.
 * @return this object to allow further assignments in one line.
 */
ExecArgs & ExecArgs::AddArgument(SubArg argument, int value, std::string seperator)
{
    return AddArgument(argument.toString(), value, std::move(seperator));
}

/**
 * @brief Add a subarg as argument to the ExecArgList.
 * This is necessary when the argument itself consists of multiple parts. E.g. smua.measurement.voltage = 3.
 * Here smua.measurement.voltage is encapsulated in a SubArg.
 * @param argument sub-argument which internally transformed into a string.
 * @param value a double value which is internally transformed to a string.
 * @param seperator the seperator which separates the value and argument.
 * @return this object to allow further assignments in one line.
 */
ExecArgs & ExecArgs::AddArgument(SubArg argument, double value, std::string seperator)
{
    return AddArgument(argument.toString(), value, std::move(seperator));
}

/**
 * @brief Add a subarg as argument to the ExecArgList.
 * This is necessary when the argument itself consists of multiple parts. E.g. smua.measurement.voltage = 3.
 * Here smua.measurement.voltage is encapsulated in a SubArg.
 * @param argument sub-argument which internally transformed into a string.
 * @param value a string which is internally transformed to a string.
 * @param seperator the seperator which separates the value and argument.
 * @return this object to allow further assignments in one line.
 */
ExecArgs & ExecArgs::AddArgument(SubArg argument, const std::string& value, const std::string& seperator)
{
    return AddArgument(argument.toString(), value, seperator);
}

/**
 * @brief Add a subarg as argument to the ExecArgList.
 * This is necessary when the argument itself consists of multiple parts. E.g. smua.measurement.voltage = 3.
 * Here smua.measurement.voltage is encapsulated in a SubArg.
 * @param argument sub-argument which internally transformed into a string.
 * @param value add a sub-argument e.g. when the value consists of multiple sub-arguments smua.OUTPUT_ON.
 * @param seperator the seperator which separates the value and argument.
 * @return this object to allow further assignments in one line.
 */
ExecArgs &ExecArgs::AddArgument(SubArg argument, SubArg& value, const std::string& seperator)
{
    return AddArgument(argument.toString(), value.toString(), seperator);
}

/**
 * @brief Transforms the list of arguments to a string which can be passed to the Exec function.
 * @return execution argument as string.
 */
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

/**
 * @brief Returns the argument type of an argument tuple. Can be integer, double or string.
 * @param argTuple Tuple of arguments consisting of ARGUMENT_TYPE, seperator and value_tuple (key, value).
 * @return ARGUMENT_TYPE (integer, double or string)
 */
/*static*/ ExecArgs::ARGUMENT_TYPE ExecArgs::GetArgumentTypeFromArgTuple(ExecArgs::ArgTuple &argTuple)
{
    return std::get<0>(argTuple);
}

/**
 * @brief Returns the seperator from the argument tuple. E.g. ' = '
 * @param argTuple Tuple of arguments consisting of ARGUMENT_TYPE, seperator and value_tuple (key, value).
 * @return seperator as string.
 */
/*static*/ std::string ExecArgs::GetSeperatorFromArgTuple(ExecArgs::ArgTuple &argTuple)
{
    return std::get<1>(argTuple);
}

/**
 * @brief Returns the argument from the argument tuple.
 * @param argTuple Tuple of arguments consisting of ARGUMENT_TYPE, seperator and value_tuple (key, value).
 * @return argument as string. For a tuple like INTEGER, '=', <voltage, 3>, voltage is returned.
 */
/*static*/ std::string ExecArgs::GetArgumentFromArgTuple(ExecArgs::ArgTuple &argTuple)
{
    std::tuple<std::string,std::string> keyValuePair = std::get<2>(argTuple);
    return std::get<0>(keyValuePair);
}

/**
 * @brief Returns the value from the argument tuple.
 * @param argTuple Tuple of arguments consisting of ARGUMENT_TYPE, seperator and value_tuple (key, value).
 * @return argument as string. For a tuple like INTEGER, '=', <voltage, 3>, 3 is returned.
 */
/*static*/ std::string ExecArgs::GetValueFromArgTuple(ExecArgs::ArgTuple &argTuple)
{
    auto keyValuePair = std::get<2>(argTuple);
    return std::get<1>(keyValuePair);
}
