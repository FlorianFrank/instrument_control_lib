#include "pybind11/pybind11.h"
#include "Device.h"
#include "devices/types/DCPowerSupply.h"
#include "devices/SPD1305.h"
#include "devices/types/SMU.h"
#include "devices/KEI2600.h"
#include "devices/KST3000.h"
#include "devices/KST33500.h"
#include "ctlib/Logging.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(py_instrument_control_lib, m) {
    py::enum_<DCPowerSupply::DC_CHANNEL>(m, "DC_CHANNEL")
            .value("CHANNEL_1", DCPowerSupply::DC_CHANNEL::CHANNEL_1)
            .value("CHANNEL_2", DCPowerSupply::DC_CHANNEL::CHANNEL_2);
}
