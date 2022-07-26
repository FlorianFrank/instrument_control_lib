#include "boost/python.hpp"
#include "Device.h"
#include "devices/types/DCPowerSupply.h"
#include "devices/SPD1305.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(libpy_icl)
{
	enum_<DCPowerSupply::DC_CHANNEL>("DC_CHANNEL")
		.value("CHANNEL_1", DCPowerSupply::DC_CHANNEL::CHANNEL_1)
		.value("CHANNEL_2", DCPowerSupply::DC_CHANNEL::CHANNEL_2);

	class_<SPD1305>("SPD1305", init<char *, int>())
		.def("Connect", &SPD1305::Connect)
		.def("setCurrent", &SPD1305::setCurrent)
		.def("getCurrent", &SPD1305::getCurrent)
		.def("Disconnect", &SPD1305::Disconnect);
}
