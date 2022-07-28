#include "boost/python.hpp"
#include "Device.h"
#include "devices/types/DCPowerSupply.h"
#include "devices/SPD1305.h"
#include "devices/KEI2600.h"
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



    class_<SPD1305>("SPD1305", init<char *, int>())
		.def("Connect", &SPD1305::Connect)
		.def("setCurrent", &SPD1305::setCurrent)
		.def("getCurrent", &SPD1305::getCurrent)
		.def("Disconnect", &SPD1305::Disconnect);


    class_<KEI2600>("KEI2600", init<char *, int>())
            .def("Connect", &KEI2600::Connect)
            .def("setCurrent", &KEI2600::Disconnect);
}
