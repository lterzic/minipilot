#include "copter/quadcopter.hpp"
#include "common/constants.hpp"
#include <cmath>

namespace mp {

quadcopter::quadcopter(const quadcopter_params_s& params, const quadcopter_actuators_s& actuators) :
    m_params(params),
    m_actuators(actuators)
{}

bool
quadcopter::actuate(const copter_actuation_s& input) noexcept
{
    throttle_s required_throttle = calculate_throttle(input);

    // TODO: Handle write failure and assert throttles are between 0 and 1
    m_actuators.fl.write_throttle(required_throttle.fl);
    m_actuators.fr.write_throttle(required_throttle.fr);
    m_actuators.bl.write_throttle(required_throttle.bl);
    m_actuators.br.write_throttle(required_throttle.br);
    
    // Assuming that the writes did not fail
    return true;
}

copter_actuation_s
quadcopter::get_actuation_limit() const noexcept
{
    return copter_actuation_s {
        .thrust = 4.f * m_params.thrust_coeff,
        .torque = {
            m_params.width_half * m_params.thrust_coeff * 2.f,
            m_params.length_half * m_params.thrust_coeff * 2.f,
            m_params.torque_coeff * 2.f
        }
    };
}

quadcopter::throttle_s
quadcopter::calculate_throttle(const copter_actuation_s& input) const noexcept
{
    const float t1 = input.thrust / m_params.thrust_coeff;
    const float t2 = input.torque(0) / (m_params.thrust_coeff * m_params.width_half);
    const float t3 = input.torque(1) / (m_params.thrust_coeff * m_params.length_half);
    const float t4 = input.torque(2) * (m_params.fl_spin_cw ? 1.f : -1.f) / m_params.torque_coeff;
    
    const float fl_sq = (t1 + t2 + t3 + t4) / 4.f;
    const float fr_sq = (t1 - t2 + t3 - t4) / 4.f;
    const float bl_sq = (t1 + t2 - t3 - t4) / 4.f;
    const float br_sq = (t1 - t2 - t3 + t4) / 4.f;

    return throttle_s {
        .fl = std::sqrt(std::max(0.f, fl_sq)),
        .fr = std::sqrt(std::max(0.f, fr_sq)),
        .bl = std::sqrt(std::max(0.f, bl_sq)),
        .br = std::sqrt(std::max(0.f, br_sq)),
    };
}

}