#include "etl.hpp"
#include <pybind11/pybind11.h>
#include <mp/copter/copter_controller_pid.hpp>

namespace py = pybind11;

PYBIND11_MODULE(copter, m, py::mod_gil_not_used()) {
    py::class_<mp::copter_params_s>(m, "copter_params_s")
        .def_readwrite("mass", &mp::copter_params_s::mass)
        .def_readwrite("moment_of_inertia", &mp::copter_params_s::moment_of_inertia)
        .def_readwrite("lin_drag_c", &mp::copter_params_s::lin_drag_c);
    
    py::class_<mp::copter_controller_pid>(m, "copter_controller_pid")
        .def(py::init<const mp::copter_params_s&>())
        .def("update", &mp::copter_controller_pid::update);
}