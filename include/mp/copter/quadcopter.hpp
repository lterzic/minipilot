#pragma once

#include "copter/copter.hpp"
#include <emblib/devices/actuators/motor.hpp>

namespace mp {

struct quadcopter_params_s : public copter_params_s {
    // Half of the width of the quad measured from the centers of left and right motors
    float width_half;
    // Half of the length of the quad measured from the centers of front and back motors
    float length_half;
    // Thrust at max throttle (assuming T = thrust_coeff * throttle^2)
    float thrust_coeff;
    // Torque at max throttle (assuming Tau = torque_coeff * throttle^2)
    float torque_coeff;
};

struct quadcopter_actuators_s {
    emblib::devices::motor &fl, &fr, &bl, &br;
};

class quadcopter : public copter {
    struct motor_values_s {
        float fl, fr, bl, br;
    };

public:
    explicit quadcopter(quadcopter_params_s& params, quadcopter_actuators_s actuators) noexcept :
        copter(params),
        m_params(params),
        m_actuators(actuators)
    {}

    /**
     * Computes the needed speeds via inverse_mma and assigns them to the appropriate motors
     */
    bool actuate(const copter_actuation_s& input) noexcept override;

private:
    /**
     * Compute the thrust based on current motor speeds
     * @note Inverse of the `actuate` method
     */
    float get_thrust() const noexcept;

    /**
     * Compute the torque based on current motor speeds
     * @note Inverse of the `actuate` method
     */
    vector3f get_torque() const noexcept;

    /**
     * Compute the motor speeds to produce the given thrust and torque
     * @todo Make static and pass the copter params as an arg
     */
    motor_values_s inverse_mma(float thrust, const vector3f& torque) const noexcept;

    /**
     * Read the current motor speeds
     * @note Assuming that all motor.read_speed calls are successful
     */
    motor_values_s read_motor_speeds(bool square = false) const noexcept;

    /**
     * Get the direction of each motor, setting 1 if CCW else -1
     */
    motor_values_s get_motor_directions() const noexcept;

private:
    quadcopter_params_s& m_params;
    quadcopter_actuators_s m_actuators;
};
    
}