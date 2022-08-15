//
// Created by florianfrank on 13.08.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_EXECARGS_H
#define INSTRUMENT_CONTROL_LIB_EXECARGS_H

#include <string>
#include <vector>
#include <tuple>

class ExecArgs
{
    enum ARGUMENT_TYPE
    {
        ARG_TYPE_INTEGER, ARG_TYPE_FLOAT, ARG_TYPE_STRING
    };

    typedef std::tuple<ARGUMENT_TYPE, std::string, std::tuple<std::string,std::string>> ArgTuple;

public:
    explicit ExecArgs() = default;

    ExecArgs* AddArgument(std::string argument, int value, std::string seperator="");
    ExecArgs* AddArgument(std::string argument, double value, std::string seperator= "");
    ExecArgs* AddArgument(std::string argument, std::string value, std::string seperator="");

    std::string GetArgumentsAsString();

private:

    ARGUMENT_TYPE GetArgumentTypeFromArgTuple(ArgTuple& argTuple);
    std::string GetSeperatorFromArgTuple(ArgTuple& argTuple);
    std::string GetArgumentFromArgTuple(ArgTuple& argTuple);
    std::string GetValueFromArgTuple(ArgTuple& argTuple);


    std::vector<std::tuple<ARGUMENT_TYPE, std::string, std::tuple<std::string,std::string>>> m_ArgList;
};


#endif //INSTRUMENT_CONTROL_LIB_EXECARGS_H
