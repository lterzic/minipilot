#include "etl.hpp"
#include "math.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>
#include <mp/copter/quadcopter.hpp>
#include <mp/copter/copter_controller_pid.hpp>

namespace py = pybind11;

PYBIND11_MODULE(copter, m, py::mod_gil_not_used()) {
    py::add_ostream_redirect(m, "copter_ostream_redirect");

    py::class_<mp::copter_params_s>(m, "copter_params_s")
        .def(py::init<float, mp::matrix3f, float, float>())
        .def_readwrite("mass", &mp::copter_params_s::mass)
        .def_readwrite("moment_of_inertia", &mp::copter_params_s::moment_of_inertia)
        .def_readwrite("lin_drag_c", &mp::copter_params_s::lin_drag_c)
        .def_readwrite("ang_drag_c", &mp::copter_params_s::ang_drag_c);

    py::class_<mp::copter_actuation_s>(m, "copter_actuation_s")
        .def_readwrite("thrust", &mp::copter_actuation_s::thrust)
        .def_readwrite("torque", &mp::copter_actuation_s::torque);

    py::class_<mp::copter_controller::angular_controls_s>(m, "angular_controls_s")
        .def(py::init<mp::vector3f, float>())
        .def_readwrite("thrust", &mp::copter_controller::angular_controls_s::thrust)
        .def_readwrite("velocity", &mp::copter_controller::angular_controls_s::angular_velocity);

    py::class_<mp::copter_controller::linear_controls_s>(m, "linear_controls_s")
        .def(py::init<mp::vector3f, float>())
        .def_readwrite("direction", &mp::copter_controller::linear_controls_s::direction)
        .def_readwrite("velocity", &mp::copter_controller::linear_controls_s::linear_velocity);
    
    py::class_<mp::copter_controller_pid>(m, "copter_controller_pid")
        .def(py::init<const mp::copter_controller_pid::params_s&, const mp::copter_params_s&>())
        .def("update", &mp::copter_controller_pid::update);

    py::class_<mp::copter_controller_pid::params_s>(m, "copter_controller_pid_params_s")
        .def(py::init<float, float, float, float, float, float, float, float, float>());
}