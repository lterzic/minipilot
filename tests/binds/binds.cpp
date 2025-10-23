#include "etl.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <mp/state/model_simple.hpp>
#include <mp/state/ekf.hpp>

namespace py = pybind11;

PYBIND11_MODULE(mpbind, m, py::mod_gil_not_used()) {
    py::class_<mp::vector3f>(m, "vector3f")
        .def(py::init([](float a, float b, float c) {
            return mp::vector3f({a, b, c});
        }));

    py::class_<mp::model>(m, "model");
    py::class_<mp::model_simple, mp::model>(m, "model_simple")
        .def(py::init<float, float>());

    py::class_<mp::state_s>(m, "state_s")
        .def_readwrite("position", &mp::state_s::position)
        .def_readwrite("velocity", &mp::state_s::velocity)
        .def_readwrite("acceleration", &mp::state_s::acceleration)
        .def_readwrite("angular_velocity", &mp::state_s::angular_velocity)
        .def_readwrite("rotationq", &mp::state_s::rotationq)
        .def_readwrite("gyroscope_drift", &mp::state_s::gyroscope_drift);

    py::class_<mp::state_estimator_algo::sensors_s>(m, "sensors_s")
        .def(py::init<>())
        .def_readwrite("accelerometer", &mp::state_estimator_algo::sensors_s::accelerometer)
        .def_readwrite("gyroscope", &mp::state_estimator_algo::sensors_s::gyroscope);
    
    py::class_<mp::ekf>(m, "ekf")
        .def(py::init<const mp::model&>())
        .def("update", &mp::ekf::update);
}