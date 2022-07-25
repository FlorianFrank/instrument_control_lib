//
// Created by florianfrank on 19.07.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_FUNCTIONGENERATOR_H
#define INSTRUMENT_CONTROL_LIB_FUNCTIONGENERATOR_H

#include "Device.h"

class FunctionGenerator : public Device
{
public:
    enum FUNCTION_TYPE {
        /* Sinus signal*/
        SIN,
        /* Square signal*/
        SQUARE,
        /* Square signal*/
        RAMP,
        /* Negative ramp signal*/
        NEGATIVE_RAM,
        /* Triangle signal*/
        TRIANGLE,
        /* Noise signal */
        NOISE,
        /* Pseudo random bit stream signal */
        PSEUDO_RANDOM_BIT_STREAM,
        /* Arbitrary signal */
        ARBITRARY
    };

    enum FUNC_CHANNEL {
        CHANNEL_A,
        CHANNEL_B
    };

    explicit FunctionGenerator(const char *ip, int timeoutInMs, PIL::Logging *logger);

    virtual PIL_ERROR_CODE turnOn(FUNC_CHANNEL channel) = 0;
    virtual PIL_ERROR_CODE turnOff(FUNC_CHANNEL channel) = 0;

    virtual PIL_ERROR_CODE setFrequency(FUNC_CHANNEL channel, double value) = 0;
    virtual PIL_ERROR_CODE setAmplitude(FUNC_CHANNEL channel, double value,  const char *constrain) = 0;
    virtual PIL_ERROR_CODE setPhase(FUNC_CHANNEL channel, double value) = 0;
    virtual PIL_ERROR_CODE setFunction(FUNCTION_TYPE functionType) = 0;
    virtual PIL_ERROR_CODE setOffset(FUNC_CHANNEL channel, double offset) = 0;

    FUNCTION_TYPE getOutputFunction() const;
    bool isOutputEnabled() const;
    double getCurrentAmplitude() const;

protected:
    FUNCTION_TYPE m_CurrentFunction;
    bool m_OutputEnabled;
    double m_Amplitude;


};


#endif //INSTRUMENT_CONTROL_LIB_FUNCTIONGENERATOR_H
