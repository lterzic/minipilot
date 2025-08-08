#pragma once

#include "model.hpp"

namespace mp {

class model_simple : public model {
public:
    explicit model_simple(
        float linear_decay_coeff,
        float angular_decay_coeff
    ) :
        m_linear_decay_coeff(linear_decay_coeff),
        m_angular_decay_coeff(angular_decay_coeff)
    {}

    vector3f get_linear_acceleration(
        const vector3f& v,
        const quaternionf& q
    ) const noexcept override
    {
        return -m_linear_decay_coeff * v;
    }

    vector3f get_angular_acceleration(
        const vector3f& v,
        const vector3f& w,
        const quaternionf& q
    ) const noexcept override
    {
        return -m_angular_decay_coeff * w;
    }

    jacobian_s get_jacobian(
        const vector3f& v,
        const vector3f& w,
        const vector4f& qv
    ) const noexcept override
    {
        return {
            .da_dv = matrix3f::diagonal(-m_linear_decay_coeff),
            .ddw_dw = matrix3f::diagonal(-m_angular_decay_coeff)
        };
    }

private:
    float m_linear_decay_coeff;
    float m_angular_decay_coeff;
};

}