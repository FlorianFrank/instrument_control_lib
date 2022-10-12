#include "pybind11/pybind11.h"

#include <iostream>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

struct Pet {
    Pet(const std::string &name) : name(name) { }
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }
    void setNameStd() { name = "Test...Test"; }

    std::string name;
};

PYBIND11_MODULE(py_instrument_control_lib, m) {
py::class_<Pet>(m, "Pet")
.def(py::init<const std::string &>())
.def("setName", &Pet::setName)
.def("getName", &Pet::getName)
.def("setNameStd", &Pet::setNameStd);
}
