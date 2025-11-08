#include "etl.hpp"
#include "math.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <mp/state/ekf.hpp>

namespace py = pybind11;

struct custom_model : public mp::model {
    std::function<mp::vector3f(const mp::vector3f&, const mp::vector3f&, const mp::quaternionf&)> m_acc;
    std::function<mp::vector3f(const mp::vector3f&, const mp::vector3f&, const mp::quaternionf&)> m_ang;
    std::function<jacobian_s(const mp::vector3f&, const mp::vector3f&, const mp::vector4f&, const mp::vector3f&)> m_jac;
    std::function<mp::matrixf<16>()> m_qmat;

    custom_model(decltype(m_acc) acc, decltype(m_ang) ang, decltype(m_jac) jac, decltype(m_qmat) qmat) :
        m_acc(acc),
        m_ang(ang),
        m_jac(jac),
        m_qmat(qmat)
    {}

    mp::vector3f get_linear_acceleration(
        const mp::vector3f& v,
        const mp::vector3f& a,
        const mp::quaternionf& q
    ) const noexcept override
    {
        return m_acc(v, a, q);
    }

    mp::vector3f get_angular_acceleration(
        const mp::vector3f& v,
        const mp::vector3f& w,
        const mp::quaternionf& q
    ) const noexcept override
    {
        return m_ang(v, w, q);
    }

    jacobian_s get_jacobian(
        const mp::vector3f& v,
        const mp::vector3f& a,
        const mp::vector4f& qv,
        const mp::vector3f& w
    ) const noexcept override
    {
        return m_jac(v, a, qv, w);
    }

    mp::matrixf<16> get_process_noise() const noexcept override
    {
        return m_qmat();
    }
};

PYBIND11_MODULE(state, m, py::mod_gil_not_used()) {
    py::class_<mp::quaternionf>(m, "quaternionf")
        .def(py::init<float, float, float, float>())
        .def("as_vector", &mp::quaternionf::as_vector)
        .def("rotate_vector", &mp::quaternionf::rotate_vec);

    py::class_<mp::state_s>(m, "state_s")
        .def(py::init<>())
        .def_readwrite("position", &mp::state_s::position)
        .def_readwrite("velocity", &mp::state_s::velocity)
        .def_readwrite("acceleration", &mp::state_s::acceleration)
        .def_readwrite("angular_velocity", &mp::state_s::angular_velocity)
        .def_readwrite("rotation", &mp::state_s::rotation)
        .def_readwrite("gyroscope_drift", &mp::state_s::gyroscope_drift);

    py::class_<mp::model>(m, "model");
    py::class_<custom_model, mp::model>(m, "custom_model")
        .def(py::init<
            decltype(custom_model::m_acc),
            decltype(custom_model::m_ang),
            decltype(custom_model::m_jac),
            decltype(custom_model::m_qmat)
        >());    

    py::class_<mp::state_estimator::sensors_s>(m, "sensors_s")
        .def(py::init<>())
        .def_readwrite("accelerometer", &mp::state_estimator::sensors_s::accelerometer)
        .def_readwrite("gyroscope", &mp::state_estimator::sensors_s::gyroscope);
    
    py::class_<mp::ekf>(m, "ekf")
        .def(py::init<const mp::model&>())
        .def("update", &mp::ekf::update);
}