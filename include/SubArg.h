#ifndef INSTRUMENT_CONTROL_LIB_SUBARG_H
#define INSTRUMENT_CONTROL_LIB_SUBARG_H

#include <vector> // std::vector
#include <tuple> // std::tuple
#include <string> // std::string

/**
 * @brief This class implements sub-arguments which can be added to a ExecArgs object.
 * This is required, when parts of the command contains a substructure, e.g. smua.voltage = 10,
 * then smu.voltage can be represented as SubArg.
 */
class SubArg
{
public:
    explicit SubArg(const std::string& arg, const std::string& prefix = "", const std::string& suffix = "");

    SubArg& AddElem(const std::string&, const std::string& prefix="", const std::string& suffix="");
    SubArg& AddElem(int value, const std::string& prefix="", const std::string& postfix="");
    SubArg& AddElem(double value, const std::string& prefix="", const std::string& postfix="");

    std::string toString();

private:
    /** Tuple consisting of parameter, prefix and suffix. **/
    std::vector<std::tuple<std::string, std::string, std::string>> m_SubArgElem;
};


#endif //INSTRUMENT_CONTROL_LIB_SUBARG_H
