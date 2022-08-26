//
// Created by liuwuhao on 27.05.21.
//

#ifndef CE_DEVICE_KST33500_H
#define CE_DEVICE_KST33500_H

#include "types/FunctionGenerator.h"

namespace PIL {
    class Logging;
}

class KST33500 : public FunctionGenerator {

public:
    explicit KST33500(const char *ip, int timeoutInMS);
    explicit KST33500(const char *ip, int timeoutInMs, PIL::Logging *logger);

    PIL_ERROR_CODE turnOn() override;
    PIL_ERROR_CODE turnOff() override;

    PIL_ERROR_CODE setFrequency(double value) override;
    PIL_ERROR_CODE setAmplitude(double value, const char *constrain) override;
    PIL_ERROR_CODE setOffset(double offset) override;
    PIL_ERROR_CODE setPhase(double value) override;
    PIL_ERROR_CODE setFunction(FUNCTION_TYPE functionType) override;

    PIL_ERROR_CODE display(std::string &text);
    PIL_ERROR_CODE displayConnection();

private:
    PIL_ERROR_CODE output(bool on);
    PIL_ERROR_CODE setPulseWidth(double value);
    std::string GetFunctionStr(FUNCTION_TYPE functionType);
};

#endif //CE_DEVICE_KST33500_H
