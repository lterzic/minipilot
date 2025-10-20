#include <pybind11/pybind11.h>
#include <mp/state/ekf.hpp>

namespace py = pybind11;

PYBIND11_MODULE(ekf, m, py::mod_gil_not_used()) {
    py::class_<mp::ekf>(m, "ekf")
        .def(py::init<const mp::model&>())
        .def("update", &mp::ekf::update);
}