#include "copter/copter_controller_pid.hpp"
#include "common/constants.hpp"

namespace mp {

copter_controller_pid::copter_controller_pid(
    const state_estimator& state_estimator,
    const copter_params_s& copter_params
) noexcept :
    m_state_estimator(state_estimator),
    m_copter_params(copter_params),
    m_angular_velocity_pid(1, 0.2, 0),
    m_linear_acceleration_pid(1.4, 0.1, 0.1)
{}



copter_controller_pid::actuation_s
copter_controller_pid::iterate(float dt) noexcept
{
    // Get current copter state
    state_s state = m_state_estimator.get_state();
    // Get current input
    control_v controls = get_controls();

    if (controls.is_type<linear_controls_s>()) {
        const vector3f& v = state.velocity;
        const auto& target = etl::get<linear_controls_s>(controls);

        // Scaling constant should be determined based on maximum velocity and maximum acceleration
        m_linear_acceleration_pid.update(target.velocity - v, dt);
        const vector3f target_a = m_linear_acceleration_pid.get_output();
        // Target thrust vector in the global frame - derived from the linear acceleration equation of the copter
        const vector3f target_thrust_g = m_copter_params.mass * (target_a - GV) + m_copter_params.lin_drag_c * v;
        // Target thrust vector in the body (local) frame
        const vector3f target_thrust_l = state.rotationq.conjugate().rotate_vec(target_thrust_g);
        // Direction of the angular velocity vector required to rotate local UP to local target thrust
        const vector3f target_w_dir = UP.cross(target_thrust_l.normalized());

        // Since the cross product is between to normalized vectors, its max magnitude is
        // 1 when the angle is PI/2, so this constant is the maximum magnitude of target w
        m_angular_target.velocity = target_w_dir * 2.f;

        // TODO: Add yaw rotation based on m_target_dir

        m_angular_target.thrust = target_thrust_g.norm();
    }

    const vector3f& w = state.angular_velocity;

    // target_dw = (target_w - w) * PID(s)
    m_angular_velocity_pid.update(m_angular_target.velocity - w, dt);
    vector3f target_dw = m_angular_velocity_pid.get_output();

    const matrix3f& I = m_copter_params.moment_of_inertia;
    const vector3f torque = I.matmul(target_dw) + w.cross(static_cast<vector3f>(I.matmul(w)));

    return {m_angular_target.thrust, torque};
}

}