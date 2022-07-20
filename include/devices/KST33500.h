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

    int turnOn(FUNC_CHANNEL channel) override;
    int turnOff(FUNC_CHANNEL channel) override;

    int setFrequency(FUNC_CHANNEL channel, double value) override;
    int setAmplitude(FUNC_CHANNEL channel, double value, const char *constrain) override;
    bool setOffset(FUNC_CHANNEL channel, double offset) override;
    int setPhase(FUNC_CHANNEL channel, double value) override;
    bool setFunction(FUNCTION_TYPE functionType) override;

    bool display(std::string &text);
    bool display_connection();

private:
    bool output(bool on);

    int offset(double value);

    int set_pulse_width(double value);

};

#endif //CE_DEVICE_KST33500_H
