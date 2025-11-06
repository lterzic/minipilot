#pragma once

#include "common/math.hpp"

namespace mp {

/**
 * Description of the kinematics of a particular model (vehicle)
 */
class model {

public:
    /**
     * Derivatives of linear and angular acceleration functions with
     * respect to state variables which might be used to calculate them
     * 
     * @note `dy_dx` means derivative (jacobian) of vector `y` with respect to vector `x`
     * @note `dw` is the angular acceleration (dw/dt)
     */
    struct jacobian_s {
        matrix3f da_dv;
        matrix3f da_da;
        matrixf<3, 4> da_dq;
        matrix3f ddw_dv;
        matrix3f ddw_dw;
        matrixf<3, 4> ddw_dq;
    };

public:
    /**
     * Calculate the model's expected acceleration in the global (inertial) reference
     * frame based on the current state (velocity and rotation) in [m/s^2]
     * @todo Can add typedef for vectors in the global and local reference frames
     * @todo Rename to `get_a`
     */
    virtual vector3f get_linear_acceleration(
        const vector3f& v,
        const vector3f& a,
        const quaternionf& q
    ) const noexcept = 0;

    /**
     * Calculate the model's expected angular acceleration based on the
     * current state in [rad/s^2]
     * @param w Angular velocity in the body frame in [rad/s]
     * @todo Rename to `get_dw`
     */
    virtual vector3f get_angular_acceleration(
        const vector3f& v,
        const vector3f& w,
        const quaternionf& q
    ) const noexcept = 0;

    /**
     * Calculate the jacobian
     */
    virtual jacobian_s get_jacobian(
        const vector3f& v,
        const vector3f& a,
        const vector4f& qv,
        const vector3f& w
    ) const noexcept = 0;

    /**
     * Get process noise
     * @todo Matrix dimension should be ekf::KALMAN_DIM, need to fix
     * circular include
     */
    virtual matrixf<16> get_process_noise() const noexcept = 0;
};

}