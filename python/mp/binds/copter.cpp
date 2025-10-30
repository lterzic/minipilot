#include "etl.hpp"
#include <pybind11/pybind11.h>
#include <mp/copter/quadcopter.hpp>
#include <mp/copter/copter_controller_pid.hpp>

namespace py = pybind11;

PYBIND11_MODULE(copter, m, py::mod_gil_not_used()) {
    py::class_<mp::copter_params_s>(m, "copter_params_s")
        .def_readwrite("mass", &mp::copter_params_s::mass)
        .def_readwrite("moment_of_inertia", &mp::copter_params_s::moment_of_inertia)
        .def_readwrite("lin_drag_c", &mp::copter_params_s::lin_drag_c);

    py::class_<mp::copter_actuation_s>(m, "copter_actuation_s")
        .def_readwrite("thrust", &mp::copter_actuation_s::thrust)
        .def_readwrite("torque", &mp::copter_actuation_s::torque);

    py::class_<mp::copter_controller::angular_controls_s>(m, "angular_controls_s")
        .def_readwrite("thrust", &mp::copter_controller::angular_controls_s::thrust)
        .def_readwrite("velocity", &mp::copter_controller::angular_controls_s::angular_velocity);

    py::class_<mp::copter_controller::linear_controls_s>(m, "linear_controls_s")
        .def_readwrite("direction", &mp::copter_controller::linear_controls_s::direction)
        .def_readwrite("velocity", &mp::copter_controller::linear_controls_s::linear_velocity);
    
    py::class_<mp::copter_controller_pid>(m, "copter_controller_pid")
        .def(py::init<const mp::copter_params_s&>())
        .def("update_angular", &mp::copter_controller_pid::update_angular)
        .def("update_linear", &mp::copter_controller_pid::update_linear);
}