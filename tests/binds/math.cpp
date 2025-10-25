#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <mp/common/math.hpp>

namespace py = pybind11;

PYBIND11_MODULE(math, m, py::mod_gil_not_used()) {
    py::class_<mp::vector3f>(m, "vector3f")
        .def(py::init([](float a, float b, float c) {
            return mp::vector3f({a, b, c});
        }))
        .def("get_base", [](const mp::vector3f& v) {
            return v.get_base();
        });
    
    py::class_<mp::quaternionf>(m, "quaternionf")
        .def(py::init<float, float, float, float>());
}