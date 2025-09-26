#include "copter/quadcopter.hpp"
#include "common/constants.hpp"
#include <cmath>

namespace mp {

bool quadcopter::actuate(float thrust, vector3f torque) noexcept
{
    motor_values_s required_throttle = inverse_mma(thrust, torque);

    // TODO: Handle write failure and assert throttles are between 0 and 1
    m_actuators.fl.write_throttle(required_throttle.fl);
    m_actuators.fr.write_throttle(required_throttle.fr);
    m_actuators.bl.write_throttle(required_throttle.bl);
    m_actuators.br.write_throttle(required_throttle.br);
    
    // Assuming that the writes did not fail
    return true;
}

quadcopter::motor_values_s quadcopter::read_motor_speeds(bool square) const noexcept
{
    motor_values_s result = {0};

    m_actuators.fl.read_throttle(result.fl);
    m_actuators.fr.read_throttle(result.fr);
    m_actuators.bl.read_throttle(result.bl);
    m_actuators.br.read_throttle(result.br);

    if (square) {
        result.fl *= result.fl;
        result.fr *= result.fr;
        result.bl *= result.bl;
        result.br *= result.br;
    }

    return result;
}

quadcopter::motor_values_s quadcopter::get_motor_directions() const noexcept
{
    motor_values_s result;

    result.fl = m_actuators.fl.get_direction() ? 1.f : -1.f;
    result.fr = m_actuators.fr.get_direction() ? 1.f : -1.f;
    result.bl = m_actuators.bl.get_direction() ? 1.f : -1.f;
    result.br = m_actuators.br.get_direction() ? 1.f : -1.f;

    return result;
}

float quadcopter::get_thrust() const noexcept
{
    motor_values_s w_sq = read_motor_speeds(true);
    return m_params.thrust_coeff * (w_sq.fl + w_sq.fr + w_sq.bl + w_sq.br);
}

vector3f quadcopter::get_torque() const noexcept
{
    motor_values_s w_sq = read_motor_speeds(true);
    motor_values_s dir = get_motor_directions();

    float torque_up = m_params.torque_coeff * (dir.fl*w_sq.fl + dir.fr*w_sq.fr + dir.bl*w_sq.bl + dir.br*w_sq.br);
    float torque_fwd = m_params.width_half * m_params.thrust_coeff * (w_sq.fl + w_sq.bl - w_sq.fr - w_sq.br);
    float torque_left = m_params.length_half * m_params.thrust_coeff * (w_sq.bl + w_sq.br - w_sq.fl - w_sq.fr);

    return UP * torque_up + FORWARD * torque_fwd + LEFT * torque_left;
}

quadcopter::motor_values_s quadcopter::inverse_mma(float thrust, const vector3f& torque) const noexcept
{
    motor_values_s dir = get_motor_directions();

    const float c_dim = m_params.width_half * m_params.length_half;
    const float c_den = 2.f * m_params.thrust_coeff * m_params.torque_coeff * c_dim;
    const float c_thrust = thrust * m_params.torque_coeff * c_dim;
    const float c_torque_up = 2.f * m_params.thrust_coeff * c_dim * torque.dot(UP);
    const float c_torque_fwd = m_params.torque_coeff * m_params.length_half * torque.dot(FORWARD);
    const float c_torque_left = m_params.torque_coeff * m_params.width_half * torque.dot(LEFT);

    const float w_bl_sq = -(c_thrust*(dir.br + dir.fl) - c_torque_up + c_torque_fwd*(dir.fl - dir.fr) + c_torque_left*(dir.br - dir.fr))/(c_den*(dir.bl - dir.br - dir.fl + dir.fr));
    const float w_br_sq = (c_thrust*(dir.bl + dir.fr) - c_torque_up + c_torque_fwd*(dir.fl - dir.fr) + c_torque_left*(dir.bl - dir.fl))/(c_den*(dir.bl - dir.br - dir.fl + dir.fr));
    const float w_fl_sq = (c_thrust*(dir.bl + dir.fr) - c_torque_up + c_torque_fwd*(dir.bl - dir.br) + c_torque_left*(dir.br - dir.fr))/(c_den*(dir.bl - dir.br - dir.fl + dir.fr));
    const float w_fr_sq = -(c_thrust*(dir.br + dir.fl) - c_torque_up + c_torque_fwd*(dir.bl - dir.br) + c_torque_left*(dir.bl - dir.fl))/(c_den*(dir.bl - dir.br - dir.fl + dir.fr));
    
    return {
        .fl = w_fl_sq < 0.f ? 0.f : std::sqrt(w_fl_sq),
        .fr = w_fr_sq < 0.f ? 0.f : std::sqrt(w_fr_sq),
        .bl = w_bl_sq < 0.f ? 0.f : std::sqrt(w_bl_sq),
        .br = w_br_sq < 0.f ? 0.f : std::sqrt(w_br_sq)
    };
}

}