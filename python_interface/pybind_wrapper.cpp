#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "Device.h"
#include "devices/types/DCPowerSupply.h"
#include "devices/SPD1305.h"
#include "devices/types/SMU.h"
#include "devices/KEI2600.h"
#include "devices/KST3000.h"
#include "devices/KST33500.h"

using namespace pybind11;

namespace py = pybind11;

PYBIND11_MODULE(py_instrument_control_lib, m) {

    enum_<PIL_ERROR_CODE>(m, "ERROR_CODE")
        .value("NO_ERROR", PIL_ERROR_CODE::PIL_NO_ERROR)
        .value("PIL_INVALID_ARGUMENTS", PIL_ERROR_CODE::PIL_INVALID_ARGUMENTS)
        .value("ERRNO", PIL_ERROR_CODE::PIL_ERRNO)
        .value("INTERFACE_CLOSED", PIL_ERROR_CODE::PIL_INTERFACE_CLOSED)
        .value("INVALID_BAUDRATE", PIL_ERROR_CODE::PIL_INVALID_BAUDRATE)
        .value("INSUFFICIENT_RESOURCES", PIL_ERROR_CODE::PIL_INSUFFICIENT_RESOURCES)
        .value("INSUFFICIENT_PERMISSIONS", PIL_ERROR_CODE::PIL_INSUFFICIENT_PERMISSIONS)
        .value("DEADLOCK_DETECTED", PIL_ERROR_CODE::PIL_DEADLOCK_DETECTED)
        .value("THREAD_NOT_JOINABLE", PIL_ERROR_CODE::PIL_THREAD_NOT_JOINABLE)
        .value("THREAD_NOT_FOUND", PIL_ERROR_CODE::PIL_THREAD_NOT_FOUND)
        .value("ONLY_PARTIALLY_READ_WRITTEN", PIL_ERROR_CODE::PIL_ONLY_PARTIALLY_READ_WRITTEN)
        .value("NO_SUCH_FILE", PIL_ERROR_CODE::PIL_NO_SUCH_FILE)
        .value("UNKNOWN_ERROR", PIL_ERROR_CODE::PIL_UNKNOWN_ERROR)
        .value("XML_PARSING_ERROR", PIL_ERROR_CODE::PIL_XML_PARSING_ERROR)
        .value("PIL_ITEM_IN_ERROR_QUEUE", PIL_ERROR_CODE::PIL_ITEM_IN_ERROR_QUEUE);

    /** DC Powersupply **/
    class_<SPD1305>(m, "SPD1305")
        .def(pybind11::init<char *, int>())
        .def("turnOn", &SPD1305::turnOn)
        .def("turnOff", &SPD1305::turnOff)
        .def("setCurrent", &SPD1305::setCurrent)
        .def("getCurrent", &SPD1305::getCurrent);

    enum_<DCPowerSupply::DC_CHANNEL>(m, "DC_CHANNEL")
        .value("CHANNEL_1", DCPowerSupply::DC_CHANNEL::CHANNEL_1)
        .value("CHANNEL_2", DCPowerSupply::DC_CHANNEL::CHANNEL_2);

    /** SMU **/
    class_<KEI2600>(m, "KEI2600")
        .def(pybind11::init<char *, int, Device::SEND_METHOD>())
        .def("enableBeep", &KEI2600::enableBeep)
        .def("disableBeep", &KEI2600::disableBeep)
        .def("beep", &KEI2600::beep)
        .def("connect", &KEI2600::Connect)
        .def("disconnect", &KEI2600::Disconnect)
        .def("turnOn", &KEI2600::turnOn)
        .def("turnOff", &KEI2600::turnOff)
        .def("measure", &KEI2600::measurePy)
        .def("setLevel", &KEI2600::setLevel)
        .def("setLimit", &KEI2600::setLimit)

        .def("enableMeasureAutoRange", &KEI2600::enableMeasureAutoRange)
        .def("disableMeasureAutoRange", &KEI2600::disableMeasureAutoRange)
        .def("enableSourceAutoRange", &KEI2600::enableSourceAutoRange)
        .def("enableMeasureAnalogFilter", &KEI2600::enableMeasureAnalogFilter)
        .def("disableMeasureAnalogFilter", &KEI2600::disableMeasureAnalogFilter)
        .def("disableSourceAutoRange", &KEI2600::disableSourceAutoRange)
        .def("setSourceRange", &KEI2600::setSourceRange)
        .def("setSenseMode", &KEI2600::setSenseMode)
        .def("getDeviceIdentifier", &KEI2600::getDeviceIdentifier)

        .def("setMeasurePLC", &KEI2600::setMeasurePLC)
        .def("setMeasureLowRange", &KEI2600::setMeasureLowRange)
        .def("setMeasureAutoZero", &KEI2600::setMeasureAutoZero)
        .def("setMeasureCount", &KEI2600::setMeasureCount)
        .def("setSourceFunction", &KEI2600::setSourceFunction)
        .def("setSourceOffMode", &KEI2600::setSourceOffMode)
        .def("setSourceSettling", &KEI2600::setSourceSettling)
        .def("enableSourceSink", &KEI2600::enableSourceSink)
        .def("disableSourceSink", &KEI2600::disableSourceSink)
        .def("displayMeasureFunction", &KEI2600::displayMeasureFunction)
        .def("getLastError", &KEI2600::getLastError)
        .def("clearErrorBuffer", &KEI2600::clearErrorBuffer)
        .def("getErrorBufferStatus", &KEI2600::getErrorBufferStatus)

        .def("sendScript", &KEI2600::sendScript)
        .def("executeScript", &KEI2600::executeScript)
        .def("sendAndExecuteScript", &KEI2600::sendAndExecuteScript)
        .def("performLinearVoltageSweep", &KEI2600::performLinearVoltageSweep)
        .def("executeBufferedScript", &KEI2600::executeBufferedScript)
        .def("getBuffer", &KEI2600::readBufferPy)
        .def("changeSendMode", &KEI2600::changeSendMode)
        .def("delay", &KEI2600::delay)
        .def_readonly("CHANNEL_A_BUFFER", &KEI2600::CHANNEL_A_BUFFER)
        .def_readonly("CHANNEL_B_BUFFER", &KEI2600::CHANNEL_B_BUFFER);

    enum_<SMU::SEND_METHOD>(m, "SEND_METHOD")
            .value("DIRECT_SEND", SMU::DIRECT_SEND)
            .value("BUFFER_ENABLED", SMU::BUFFER_ENABLED);

    enum_<SMU::SMU_CHANNEL>(m, "SMU_CHANNEL")
            .value("CHANNEL_A", SMU::CHANNEL_A)
            .value("CHANNEL_B", SMU::CHANNEL_B);

    enum_<SMU::UNIT>(m, "SMU_UNIT")
            .value("VOLTAGE", SMU::VOLTAGE)
            .value("CURRENT", SMU::CURRENT)
            .value("RESISTANCE", SMU::RESISTANCE)
            .value("POWER", SMU::POWER);

    enum_<SMU::AUTOZERO>(m, "SMU_AUTOZERO")
            .value("OFF", SMU::OFF)
            .value("ONCE", SMU::ONCE)
            .value("AUTO", SMU::AUTO);

    enum_<SMU::SRC_FUNC>(m, "SRC_FUNC")
            .value("DC_AMPS", SMU::DC_AMPS)
            .value("DC_VOLTS", SMU::DC_VOLTS);

    enum_<SMU::SRC_OFF_MODE>(m, "SRC_OFF_MODE")
            .value("NORMAL", SMU::OUTPUT_NORMAL)
            .value("ZERO", SMU::OUTPUT_ZERO)
            .value("HIGH_Z", SMU::OUTPUT_HIGH_Z);

    enum_<SMU::SRC_SETTLING>(m, "SRC_SETTLING")
            .value("SMOOTH", SMU::SMOOTH)
            .value("FAST_RANGE", SMU::FAST_RANGE)
            .value("FAST_POLARITY", SMU::FAST_POLARITY)
            .value("DIRECT_IRANGE", SMU::DIRECT_IRANGE)
            .value("SMOOTH_100NA", SMU::SMOOTH_100NA)
            .value("FAST_ALL", SMU::FAST_ALL);

    enum_<SMU::SMU_DISPLAY>(m, "SMU_DISPLAY")
            .value("DC_AMPS", SMU::MEASURE_DC_AMPS)
            .value("DC_VOLTS", SMU::MEASURE_DC_VOLTS)
            .value("OHMS", SMU::MEASURE_OHMS)
            .value("WATTS", SMU::MEASURE_WATTS);

    enum_<SMU::SMU_SENSE>(m, "SMU_SENSE")
            .value("LOCAL", SMU::SMU_SENSE::LOCAL)
            .value("REMOTE", SMU::SMU_SENSE::REMOTE)
            .value("CALIBRATION", SMU::SMU_SENSE::CALIBRATION);

    /** Oscilloscope**/
    class_<KST3000>(m, "KST3000")
        .def(pybind11::init<char *, int>())
        .def("connect", &KST3000::Connect)
        .def("disconnect", &KST3000::Disconnect)
        .def("run", &KST3000::run)
        .def("stop", &KST3000::stop)
        .def("single", &KST3000::single)
        .def("autoScale", &KST3000::autoScale)
        .def("setTimeRange", &KST3000::setTimeRange)
        .def("setChannelOffset", &KST3000::setChannelOffset)
        .def("setChannelScale", &KST3000::setChannelScale)
        .def("setChannelRange", &KST3000::setChannelRange)
        .def("setTriggerEdge", &KST3000::setTriggerEdge)
        .def("setTriggerSource", &KST3000::setTriggerSource)
        .def("setTimeDelay", &KST3000::setTimeDelay)
        .def("setWaveformSource", &KST3000::setWaveformSource)
        .def("getWaveformPreamble", &KST3000::getWaveformPreamble)
        .def("getWaveformPoints", &KST3000::getWaveformPoints)
        .def("setWaveformPoints", &KST3000::setWaveformPoints)
        .def("setWaveformPointsMode", &KST3000::setWaveformPointsMode)
        .def("setWaveformFormat", &KST3000::setWaveformFormat)
        .def("saveWaveformData", &KST3000::saveWaveformData)
        .def("getRealData", &KST3000::getRealDataPy)
        .def("digitize", &KST3000::digitize)
        .def("getSystemSetup", &KST3000::getSystemSetup)
        .def("setDisplayMode", &KST3000::setDisplayMode)
        .def("displayConnection", &KST3000::displayConnection)
        .def("setChannelDisplay", &KST3000::setChannelDisplay);


    enum_<Oscilloscope::OSC_CHANNEL>(m, "OSC_CHANNEL")
        .value("CHANNEL_1", Oscilloscope::CHANNEL_1)
        .value("CHANNEL_2", Oscilloscope::CHANNEL_2)
        .value("CHANNEL_3", Oscilloscope::CHANNEL_3)
        .value("CHANNEL_4", Oscilloscope::CHANNEL_4);

    enum_<Oscilloscope::TRIGGER_EDGE>(m, "TRIGGER_EDGE")
        .value("POS_EDGE", Oscilloscope::POS_EDGE)
        .value("NEG_EDGE", Oscilloscope::NEG_EDGE)
        .value("EITHER", Oscilloscope::EITHER)
        .value("ALTERNATING", Oscilloscope::ALTERNATING);

    enum_<Oscilloscope::DISPLAY_MODES>(m, "DISPLAY_MODES")
        .value("MAIN", Oscilloscope::MAIN)
        .value("WIND", Oscilloscope::WIND)
        .value("XY", Oscilloscope::XY)
        .value("ROLL", Oscilloscope::ROLL);

    enum_<Oscilloscope::FILE_FORMAT>(m, "FILE_FORMAT")
        .value("ASCII", Oscilloscope::ASCII)
        .value("WORD", Oscilloscope::WORD)
        .value("BYTE", Oscilloscope::BYTE);

    enum_<Oscilloscope::VOLTAGE_UNIT>(m, "VOLTAGE_UNIT")
        .value("VOLT", Oscilloscope::VOLT)
        .value("MILLI_VOLT", Oscilloscope::MILLI_VOLT);

    /** Function Generator **/
    class_<KST33500>(m, "KST33500")
            .def(pybind11::init<char *, int>())
            .def("connect", &KST33500::Connect)
            .def("disconnect", &KST33500::Disconnect)
            .def("turnOn", &KST33500::turnOn)
            .def("turnOff", &KST33500::turnOff)
            .def("setFrequency", &KST33500::setFrequency)
            .def("setAmplitude", &KST33500::setAmplitude)
            .def("setOffset", &KST33500::setOffset)
            .def("setPhase", &KST33500::setPhase)
            .def("setFunction", &KST33500::setFunction)
            .def("display", &KST33500::display)
            .def("display", &KST33500::displayConnection);

    enum_<FunctionGenerator::FUNCTION_TYPE>(m, "FUNCTION_TYPE")
        .value("SIN", FunctionGenerator::SIN)
        .value("SQUARE", FunctionGenerator::SQUARE)
        .value("RAMP", FunctionGenerator::RAMP)
        .value("NEGATIVE_RAM", FunctionGenerator::NEGATIVE_RAM)
        .value("TRIANGLE", FunctionGenerator::TRIANGLE)
        .value("NOISE", FunctionGenerator::NOISE)
        .value("PSEUDO_RANDOM_BIT_STREAM", FunctionGenerator::PSEUDO_RANDOM_BIT_STREAM)
        .value("ARBITRARY", FunctionGenerator::ARBITRARY)
        .value("DC_VOLTAGE", FunctionGenerator::DC_VOLTAGE);

    enum_<FunctionGenerator::FUNC_CHANNEL>(m, "FUNCTION_CHANNEL")
            .value("CHANNEL_A", FunctionGenerator::CHANNEL_A)
            .value("CHANNEL_B", FunctionGenerator::CHANNEL_B);

    class_<PIL::Logging>(m, "Logging")
            .def(pybind11::init<>())
            .def(pybind11::init<PIL::Level, std::string*>())
            .def("LogMessage", &PIL::Logging::Log)
            .def("SetLogLevel", &PIL::Logging::SetLogLevel);

    enum_<PIL::Level>(m, "Log_Level")
            .value("INFO", PIL::INFO)
            .value("DEBUG", PIL::DEBUG)
            .value("WARNING", PIL::WARNING)
            .value("ERROR", PIL::ERROR);
}
