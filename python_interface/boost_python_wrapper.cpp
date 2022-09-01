#include "boost/python.hpp"
#include "Device.h"
#include "devices/types/DCPowerSupply.h"
#include "devices/SPD1305.h"
#include "devices/types/SMU.h"
#include "devices/KEI2600.h"
#include "devices/KST3000.h"
#include "devices/KST33500.h"
#include "ctlib/Logging.hpp"


BOOST_PYTHON_MODULE(py_instrument_control_lib)
{
   // Py_Initialize();
    using namespace boost::python;

/*    class_<Device>("Device", init<char *, int>());
    class_< SMU , bases<Device>, boost::noncopyable>("SMU", no_init);
    class_< FunctionGenerator , bases<Device>, boost::noncopyable>("FunctionGenerator", no_init);
    class_< DCPowerSupply , bases<Device>, boost::noncopyable>("DCPowerSupply", no_init);*/

    enum_<DCPowerSupply::DC_CHANNEL>("DC_CHANNEL")
		.value("CHANNEL_1", DCPowerSupply::DC_CHANNEL::CHANNEL_1)
		.value("CHANNEL_2", DCPowerSupply::DC_CHANNEL::CHANNEL_2);

    enum_<PIL_ERROR_CODE>("ERROR_CODE")
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

    enum_<SMU::SMU_CHANNEL>("SMU_CHANNEL")
            .value("CHANNEL_A", SMU::CHANNEL_A)
            .value("CHANNEL_B", SMU::CHANNEL_B);

    enum_<SMU::UNIT>("SMU_UNIT")
            .value("VOLTAGE", SMU::VOLTAGE)
            .value("CURRENT", SMU::CURRENT)
            .value("RESISTANCE", SMU::RESISTANCE)
            .value("POWER", SMU::POWER);

    enum_<SMU::AUTOZERO>("SMU_AUTOZERO")
            .value("OFF", SMU::OFF)
            .value("ONCE", SMU::ONCE)
            .value("AUTO", SMU::AUTO);

    enum_<SMU::SRC_FUNC>("SRC_FUNC")
            .value("DC_AMPS", SMU::DC_AMPS)
            .value("DC_VOLTS", SMU::DC_VOLTS);

    enum_<SMU::SRC_OFF_MODE>("SRC_OFF_MODE")
            .value("NORMAL", SMU::OUTPUT_NORMAL)
            .value("ZERO", SMU::OUTPUT_ZERO)
            .value("HIGH_Z", SMU::OUTPUT_HIGH_Z);

    enum_<SMU::SRC_SETTLING>("SRC_SETTLING")
            .value("SMOOTH", SMU::SMOOTH)
            .value("FAST_RANGE", SMU::FAST_RANGE)
            .value("FAST_POLARITY", SMU::FAST_POLARITY)
            .value("DIRECT_IRANGE", SMU::DIRECT_IRANGE)
            .value("SMOOTH_100NA", SMU::SMOOTH_100NA)
            .value("FAST_ALL", SMU::FAST_ALL);

    enum_<SMU::SMU_DISPLAY>("SMU_DISPLAY")
            .value("DC_AMPS", SMU::MEASURE_DC_AMPS)
            .value("DC_VOLTS", SMU::MEASURE_DC_VOLTS)
            .value("OHMS", SMU::MEASURE_OHMS)
            .value("WATTS", SMU::MEASURE_WATTS);

    enum_<SMU::SMU_SENSE>("SMU_SENSE")
            .value("LOCAL", SMU::SMU_SENSE::LOCAL)
            .value("REMOTE", SMU::SMU_SENSE::REMOTE)
            .value("CALIBRATION", SMU::SMU_SENSE::CALIBRATION);

    class_<KEI2600>("KEI2600", init<char *, int>())
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
            .def("selectLocalSense", &KEI2600::setSenseMode)
            .def("getDeviceIdentifier", &KEI2600::GetDeviceIdentifier)

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
            .def("getErrorBufferStatus", &KEI2600::getErrorBufferStatus);





    enum_<FunctionGenerator::FUNCTION_TYPE>("FUNCTION_TYPE")
            .value("SIN", FunctionGenerator::SIN)
            .value("SQUARE", FunctionGenerator::SQUARE)
            .value("RAMP", FunctionGenerator::RAMP)
            .value("NEGATIVE_RAM", FunctionGenerator::NEGATIVE_RAM)
            .value("TRIANGLE", FunctionGenerator::TRIANGLE)
            .value("NOISE", FunctionGenerator::NOISE)
            .value("PSEUDO_RANDOM_BIT_STREAM", FunctionGenerator::PSEUDO_RANDOM_BIT_STREAM)
            .value("ARBITRARY", FunctionGenerator::ARBITRARY);

    enum_<FunctionGenerator::FUNC_CHANNEL>("FUNCTION_CHANNEL")
            .value("CHANNEL_A", FunctionGenerator::CHANNEL_A)
            .value("CHANNEL_B", FunctionGenerator::CHANNEL_B);


    enum_<Oscilloscope::OSC_CHANNEL>("OSC_CHANNEL")
            .value("CHANNEL_1", Oscilloscope::CHANNEL_1)
            .value("CHANNEL_2", Oscilloscope::CHANNEL_2)
            .value("CHANNEL_3", Oscilloscope::CHANNEL_3)
            .value("CHANNEL_4", Oscilloscope::CHANNEL_4);

    enum_<Oscilloscope::TRIGGER_EDGE>("TRIGGER_EDGE")
            .value("POS_EDGE", Oscilloscope::POS_EDGE)
            .value("NEG_EDGE", Oscilloscope::NEG_EDGE)
            .value("EITHER", Oscilloscope::EITHER)
            .value("ALTERNATING", Oscilloscope::ALTERNATING);

    enum_<Oscilloscope::DISPLAY_MODES>("DISPLAY_MODES")
            .value("MAIN", Oscilloscope::MAIN)
            .value("WIND", Oscilloscope::WIND)
            .value("XY", Oscilloscope::XY)
            .value("ROLL", Oscilloscope::ROLL);

    enum_<Oscilloscope::FILE_FORMAT>("FILE_FORMAT")
            .value("ASCII", Oscilloscope::ASCII)
            .value("WORD", Oscilloscope::WORD)
            .value("BYTE", Oscilloscope::BYTE);

    enum_<Oscilloscope::VOLTAGE_UNIT>("VOLTAGE_UNIT")
            .value("VOLT", Oscilloscope::VOLT)
            .value("MILLI_VOLT", Oscilloscope::MILLI_VOLT);



}
