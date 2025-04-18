#include "copter.hpp"
#include "util/constants.hpp"
#include "util/logger.hpp"

namespace mp {

// Coefficient when the copter is grounded to simulate
// the effect of ground resisting copter movement
static constexpr float COPTER_FRICTION_COEFF = 5.f;

vector3f copter::get_linear_acceleration(
    const vector3f& v,
    const quaternionf& q
) const noexcept
{
    // If grounded return acceleration due to friction to
    // minimize any velocity generated by the state estimator
    if (m_grounded)
        return -COPTER_FRICTION_COEFF / m_params.mass * v;
        
    const vector3f thrust_force = get_thrust() * q.rotate_vec(UP);
    const vector3f drag_force = -m_params.lin_drag_c * v;

    return GV + (thrust_force + drag_force) / m_params.mass;
}

vector3f copter::get_angular_acceleration(
    const vector3f& v,
    const vector3f& w,
    const quaternionf& q
) const noexcept
{
    if (m_grounded)
        return vector3f(0);

    const matrix3f& I = m_params.moment_of_inertia;
    const vector3f I_w = I.matmul(w);

    return (get_torque() - w.cross(I_w)).matdivl(I);
}

copter::jacobian_s copter::get_jacobian(
    const vector3f& v,
    const vector3f& w,
    const vector4f& qv
) const noexcept
{
    if (m_grounded)
        return jacobian_s {
            .da_dv = matrixf<3>::diagonal(-COPTER_FRICTION_COEFF / m_params.mass),
            .da_dq = matrixf<3, 4>(0),
            .ddw_dv = matrixf<3>(0),
            .ddw_dw = matrixf<3>(0),
            .ddw_dq = matrixf<3, 4>(0)
        };

    const float cd = m_params.lin_drag_c;
    const float m = m_params.mass;
    const float T = get_thrust();

    const auto& I = m_params.moment_of_inertia;
    const float Ix = I(0, 0), Iy = I(1, 1), Iz = I(2, 2);
    
    const float qw = qv(0), qx = qv(1), qy = qv(2), qz = qv(3);
    const float wx = w(0), wy = w(1), wz = w(2);

    // Simplified model of the inertia matrix is used (only diagonal elements)
    return jacobian_s {
        .da_dv = matrix3f::diagonal(-cd/m),
        .da_dq = (2 * T / m) * matrixf<3, 4> {
            {qy, qz, qw, qx},
            {-qx, -qw, qz, qy},
            {qw, -qx, -qy, qz}
        },
        .ddw_dv = matrixf<3>(0),
        .ddw_dw = matrixf<3> {
            {0, (Iy-Iz)*wz/Ix, (Iy-Iz)*wy/Ix},
            {(Iz-Ix)*wz/Iy, 0, (Iz-Ix)*wx/Iy},
            {(Ix-Iy)*wy/Iz, (Ix-Iy)*wx/Iz, 0}
        },
        .ddw_dq = matrixf<3, 4>(0)
    };
}

void copter::update_grounded(const state_s& state) noexcept
{
    static constexpr float TAKEOFF_ACCELERATION_THRESHOLD = 0.015f;
    static constexpr float STATIONARY_SPEED_SQ_THRESHOLD = 0.01f;
    static constexpr float STATIONARY_ACC_MINIMUM_DIFF = 1.f;

    // Check to see if we are most likely on ground
    if (m_grounded) {
        if (state.acceleration.dot(UP) > TAKEOFF_ACCELERATION_THRESHOLD) {
            m_grounded = false;
            log_info("Copter takeoff!");
            float mass = get_thrust() / G;
            // TODO: Assign copter mass to m_params
            log_info("Calculated copter mass: ", mass);
        }
    } else {
        bool stationary = state.velocity.norm_sq() < STATIONARY_SPEED_SQ_THRESHOLD;

        // This expected acceleration is calculated assuming the grounded is false
        // since we're in this branch of the if expression
        const vector3f acceleration_expected = get_linear_acceleration(state.velocity, state.rotationq);
        const vector3f acceleration_diff = state.acceleration - acceleration_expected;

        // If there is less acceleration downwards than expected and we're stationary,
        // we're probably grounded
        // if (acceleration_diff.dot(DOWN) < STATIONARY_ACC_MINIMUM_DIFF && stationary) {
        //     m_grounded = true;
        //     log_info("Copter landing!");
        // }
    }
}

void copter::update(const state_s& state, float dt) noexcept
{
    update_grounded(state);
    
    m_controller.update(state, dt);
    actuate(m_controller.get_thrust(), m_controller.get_torque());
}

bool copter::handle_command(const mp_pb_Command& command) noexcept
{
    if (command.which_command_type != mp_pb_Command_copter_command_tag) {
        return false;
    }

    // Result of command execution
    bool command_status = false;
    const mp_pb_vehicles_CopterCommand& copter_command = command.command_type.copter_command;
    
    switch (copter_command.which_command_type) {
    case mp_pb_vehicles_CopterCommand_set_angular_velocity_tag: {
        const auto& w = copter_command.command_type.set_angular_velocity.angular_velocity;
        const float thrust = copter_command.command_type.set_angular_velocity.thrust;
        command_status = m_controller.set_target_w({w.x, w.y, w.z}, thrust);
        break;
    }
    case mp_pb_vehicles_CopterCommand_set_linear_velocity_tag: {
        const auto& v = copter_command.command_type.set_linear_velocity.velocity;
        const float dir = copter_command.command_type.set_linear_velocity.direction;
        command_status = m_controller.set_target_v({v.x, v.y, v.z}, dir);
        break;
    }
    default:
        command_status = false;
    }
    return command_status;
}

}