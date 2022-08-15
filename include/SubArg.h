//
// Created by florianfrank on 15.08.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_SUBARG_H
#define INSTRUMENT_CONTROL_LIB_SUBARG_H

#include <vector>
#include <tuple>
#include <string>

class SubArg {
public:
    explicit SubArg(std::string arg);
    SubArg& AddElem(std::string, std::string prefix="", std::string postfix="");
    SubArg& AddElem(int value, std::string prefix="", std::string postfix="");
    SubArg& AddElem(double value, std::string prefix="", std::string postfix="");

    std::string toString();

private:
    std::vector<std::tuple<std::string, std::string, std::string>> m_SubArgElem;
};


#endif //INSTRUMENT_CONTROL_LIB_SUBARG_H
