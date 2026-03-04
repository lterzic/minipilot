#include "matrix.hpp"
#include "vector.hpp"
#include "quaternion.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(math, m, py::mod_gil_not_used()) {
    py::class_<mp::quaternionf>(m, "quaternionf")
        .def(py::init<float, float, float, float>())
        .def("get_real", &mp::quaternionf::get_real)
        .def("get_imag", &mp::quaternionf::get_imag)
        .def("conjugate", &mp::quaternionf::conjugate)
        .def("as_vector", &mp::quaternionf::as_vector)
        .def("rotate_vector", &mp::quaternionf::rotate_vec);
}