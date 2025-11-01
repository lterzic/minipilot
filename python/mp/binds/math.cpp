#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <mp/common/math.hpp>

namespace py = pybind11;

PYBIND11_MODULE(math, m, py::mod_gil_not_used()) {
    py::class_<mp::matrix3f>(m, "matrix3f")
        .def(py::init<const Eigen::Matrix3f&>());
    py::class_<mp::vector3f>(m, "vector3f")
        .def(py::init([](float a, float b, float c) {
            return mp::vector3f({a, b, c});
        }))
        .def(py::init([](const Eigen::Matrix<float, 3, 1>& eigenv) {
            return mp::vector3f({eigenv(0), eigenv(1), eigenv(2)});
        }))
        .def("get_base", [](const mp::vector3f& v) {
            return v.get_base();
        })
        .def("as_diagonal", &mp::vector3f::as_diagonal);

    // Used for ekf process noise matrix
    py::class_<mp::matrixf<16>>(m, "matrix16f")
        .def(py::init([](float v, float a, float q, float w, float wd) {
            return mp::vectorf<16>({
                v, v, v,
                a, a, a,
                q, q, q, q,
                w, w, w,
                wd, wd, wd
            }).as_diagonal();
        }));
    
    py::class_<mp::quaternionf>(m, "quaternionf")
        .def(py::init<float, float, float, float>())
        .def("as_vector", [](const mp::quaternionf& q) {
            return q.as_vector().get_base();
        });
}