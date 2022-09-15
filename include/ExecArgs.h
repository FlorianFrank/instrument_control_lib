/**
 * @copyright University of Passau - Chair of Computer Engineering
 * @authors Florian Frank
 */
#ifndef INSTRUMENT_CONTROL_LIB_EXEC_ARGS_H
#define INSTRUMENT_CONTROL_LIB_EXEC_ARGS_H

#include "SubArg.h"

/**
 * @brief ExecArgs class is used to create execution arguments based on a parameter seperator value scheme.
 * Can be used to dynamically add new types of commands. E.g. voltage = 3
 */
class ExecArgs
{
    /**
     * @brief This enum is used to later identify the type of the value.
     * This is necessary because all arguments are transformed to strings.
     */
    enum ARGUMENT_TYPE
    {
        ARG_TYPE_INTEGER,
        ARG_TYPE_FLOAT,
        ARG_TYPE_STRING
    };

    /** Argument tuple consisting of type, seperator, <key, value>. **/
    typedef std::tuple<ARGUMENT_TYPE, std::string, std::tuple<std::string,std::string>> ArgTuple;

public:
    explicit ExecArgs() = default;

    ExecArgs & AddArgument(std::string argument, int value, std::string seperator= "");
    ExecArgs & AddArgument(std::string argument, double value, std::string seperator= "");
    ExecArgs & AddArgument(const std::string& argument, const std::string& value, const std::string& seperator= "");

    ExecArgs & AddArgument(SubArg argument, int value, std::string seperator= "");
    ExecArgs & AddArgument(SubArg argument, double value, std::string seperator= "");
    ExecArgs & AddArgument(SubArg argument, const std::string& value, const std::string& seperator= "");

    ExecArgs & AddArgument(SubArg argument, SubArg& value, const std::string& seperator= "");

    std::string GetArgumentsAsString();

private:

    static ARGUMENT_TYPE GetArgumentTypeFromArgTuple(ArgTuple& argTuple);
    static std::string GetSeperatorFromArgTuple(ArgTuple& argTuple);
    static std::string GetArgumentFromArgTuple(ArgTuple& argTuple);
    static std::string GetValueFromArgTuple(ArgTuple& argTuple);

    std::vector<std::tuple<ARGUMENT_TYPE, std::string, std::tuple<std::string,std::string>>> m_ArgList;
};
#endif //INSTRUMENT_CONTROL_LIB_EXEC_ARGS_H
