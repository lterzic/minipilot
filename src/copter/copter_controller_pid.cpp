#include "copter/copter_controller_pid.hpp"
#include "common/constants.hpp"

namespace mp {

copter_controller_pid::copter_controller_pid(
    const copter_params_s& copter_params
) noexcept :
    m_copter_params(copter_params),
    m_angular_velocity_pid(1, 0.2, 0),
    m_linear_acceleration_pid(1.4, 0.1, 0.1)
{}

copter_actuation_s
copter_controller_pid::update_angular(const angular_controls_s& input, const state_s& state, float dt) noexcept
{
    const vector3f& w = state.angular_velocity;
    const matrix3f& I = m_copter_params.moment_of_inertia;

    // target_dw = (target_w - w) * PID(s)
    m_angular_velocity_pid.update(input.angular_velocity - w, dt);
    const vector3f target_dw = m_angular_velocity_pid.get_output();
    const vector3f torque = I.matmul(target_dw) + w.cross(static_cast<vector3f>(I.matmul(w)));

    return {input.thrust, torque};
}

copter_actuation_s
copter_controller_pid::update_linear(const linear_controls_s& input, const state_s& state, float dt) noexcept
{
    const vector3f& v = state.velocity;

    // Scaling constant should be determined based on maximum velocity and maximum acceleration
    m_linear_acceleration_pid.update(input.linear_velocity - v, dt);
    const vector3f target_a = m_linear_acceleration_pid.get_output();
    // Target thrust vector in the global frame - derived from the linear acceleration equation of the copter
    const vector3f target_thrust_g = m_copter_params.mass * (target_a - GV) + m_copter_params.lin_drag_c * v;
    // Target thrust vector in the body (local) frame
    const vector3f target_thrust_l = state.rotationq.conjugate().rotate_vec(target_thrust_g);
    // Direction of the angular velocity vector required to rotate local UP to local target thrust
    const vector3f target_w_dir = UP.cross(target_thrust_l.normalized());

    // TODO: Add yaw rotation based on m_target_dir

    const angular_controls_s angular {
        // Since the cross product is between to normalized vectors, its max magnitude is
        // 1 when the angle is PI/2, so this constant is the maximum magnitude of target w
        .angular_velocity = target_w_dir * 2.f,
        // TODO: Fix thrust to not overshoot during rotation
        .thrust = target_thrust_g.norm()
    };

    // Run the angular control part of the algorithm
    return update_angular(angular, state, dt);
}

}