#include "etl.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <mp/state/model_simple.hpp>
#include <mp/state/ekf.hpp>

namespace py = pybind11;

PYBIND11_MODULE(mpbind, m, py::mod_gil_not_used()) {
    py::class_<mp::model>(m, "model");
    py::class_<mp::model_simple, mp::model>(m, "model_simple")
        .def(py::init<float, float>());

    py::class_<mp::state_estimator_algo::sensors_s>(m, "sensors_s")
        .def(py::init<>())
        .def_readwrite("accelerometer", &mp::state_estimator_algo::sensors_s::accelerometer)
        .def_readwrite("gyroscope", &mp::state_estimator_algo::sensors_s::gyroscope);
    
    py::class_<mp::ekf>(m, "ekf")
        .def(py::init<const mp::model&>())
        .def("update", &mp::ekf::update);
}