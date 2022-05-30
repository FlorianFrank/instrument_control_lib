//
// Created by liuwuhao on 27.05.21.
//

#ifndef CE_DEVICE_KST33500_H
#define CE_DEVICE_KST33500_H

#include "Device.h"


class KST33500 : public Device {

public:
    enum FunctionType {
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

    explicit KST33500(const char *ip);

    bool display(std::string &text);

    bool display_connection();

    bool setFunction(FunctionType functionType);

    bool frequency(double value);

    bool voltage(double value, const char *constrain = "");


    bool enableOutput();

    bool disableOutput();

    bool phase(std::string &value);


    // Getters
    FunctionType getOutputFunction() const;

    bool isOutputEnabled() const;

    double getCurrentAmplitude() const;

private:
    bool output(bool on);

    FunctionType m_CurrentFunction;
    bool m_OutputEnabled;
    double m_Amplitude;
};

#endif //CE_DEVICE_KST33500_H
