#include "boost/python.hpp"
#include "Device.h"
#include "devices/types/DCPowerSupply.h"
#include "devices/SPD1305.h"
#include "devices/types/SMU.h"
#include "devices/KEI2600.h"
#include "devices/KST33500.h"
#include "ctlib/Logging.hpp"

using namespace boost::python;

BOOST_PYTHON_MODULE(libpy_icl)
{
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
            .value("XML_PARSING_ERROR", PIL_ERROR_CODE::PIL_XML_PARSING_ERROR);

    enum_<SMU::SMU_CHANNEL>("SMU_CHANNEL")
            .value("CHANNEL_A", SMU::CHANNEL_A)
            .value("CHANNEL_B", SMU::CHANNEL_B);

    enum_<SMU::UNIT>("SMU_UNIT")
            .value("VOLTAGE", SMU::VOLTAGE)
            .value("CURRENT", SMU::CURRENT)
            .value("RESISTANCE", SMU::RESISTANCE)
            .value("POWER", SMU::POWER);

    class_<KEI2600>("KEI2600", init<char *, int>())
            .def("connect", &KEI2600::Connect)
            .def("disconnect", &KEI2600::Disconnect)
            .def("turnOn", &KEI2600::turnOn)
            .def("turnOff", &KEI2600::turnOff)
            .def("measure", &KEI2600::measurePy)
            .def("setLevel", &KEI2600::setLevel)
            .def("enableMeasureAutoRange", &KEI2600::enableMeasureAutoRange)
            .def("disableMeasureAutoRange", &KEI2600::disableMeasureAutoRange)
            .def("enableSourceAutoRange", &KEI2600::enableSourceAutoRange)
            .def("disableSourceAutoRange", &KEI2600::disableSourceAutoRange)
            .def("setSourceRange", &KEI2600::setSourceRange)
            .def("selectLocalSense", &KEI2600::selectLocalSense)
            .def("selectRemoteSense", &KEI2600::selectRemoteSense)
            .def("getDeviceIdentifier", &KEI2600::GetDeviceIdentifier)
            ;

    enum_<FunctionGenerator::FUNCTION_TYPE>("FUNCTION_TYPE")
            .value("SIN", FunctionGenerator::SIN)
            .value("SIN", FunctionGenerator::SQUARE)
            .value("SIN", FunctionGenerator::RAMP)
            .value("SIN", FunctionGenerator::NEGATIVE_RAM)
            .value("SIN", FunctionGenerator::TRIANGLE)
            .value("SIN", FunctionGenerator::NOISE)
            .value("SIN", FunctionGenerator::PSEUDO_RANDOM_BIT_STREAM)
            .value("SIN", FunctionGenerator::ARBITRARY);

    enum_<FunctionGenerator::FUNC_CHANNEL>("FUNCTION_CHANNEL")
            .value("CHANNEL_A", FunctionGenerator::CHANNEL_A)
            .value("CHANNEL_B", FunctionGenerator::CHANNEL_B);

    class_<KST33500>("KST33500", init<char*, int>())
            .def("connect", &KST33500::Connect)
            .def("disconnect", &KST33500::Disconnect)
            .def("turnOn", &KST33500::turnOn)
            .def("turnOff", &KST33500::turnOff)
            .def("setFrequency", &KST33500::setFrequency)
            .def("setAmplitude", &KST33500::setAmplitude)
            .def("setOffset", &KST33500::setOffset)
            .def("setFunction", &KST33500::setFunction)
            .def("setPhase", &KST33500::setPhase)
            .def("display", &KST33500::display)
            .def("displayConnection", &KST33500::displayConnection);


    class_<SPD1305>("SPD1305", init<char *, int>())
            .def("Connect", &SPD1305::Connect)
            .def("setCurrent", &SPD1305::setCurrent)
            .def("getCurrent", &SPD1305::getCurrent)
            .def("Disconnect", &SPD1305::Disconnect);

}
