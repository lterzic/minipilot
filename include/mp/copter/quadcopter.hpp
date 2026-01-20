#pragma once

#include "copter.hpp"
#include "actuators/motor.hpp"

namespace mp {

struct quadcopter_params_s {
    // Half of the width of the quad measured from the centers of left and right motors
    float width_half;
    // Half of the length of the quad measured from the centers of front and back motors
    float length_half;
    // Thrust at max throttle (assuming Thrust = thrust_coeff * throttle^2)
    float thrust_coeff;
    // Torque at max throttle (assuming Torque = torque_coeff * throttle^2)
    float torque_coeff;
    // Is direction of the front left motor clockwise - determines motor directions
    bool fl_spin_cw;
};

struct quadcopter_actuators_s {
    motor& fl;
    motor& fr;
    motor& bl;
    motor& br;
};

/**
 * Quadcopter in the X-configuration.
 */
class quadcopter : public copter {
public:
    struct throttle_s {
        float fl, fr, bl, br;
    };

public:
    quadcopter(const quadcopter_params_s& params, const quadcopter_actuators_s& actuators);

    /**
     * Compute and set the appropriate motor speeds to achieve the target actuation.
     */
    bool actuate(const copter_actuation_s& input) noexcept override;

    /**
     * Max thrust and torque producable by the 4 prop motors.
     */
    copter_actuation_s get_actuation_limit() const noexcept override;

    /**
     * Calculate the speeds of the 4 motors to achieve the input thrust and torque.
     * @todo Add support for a single motor failure.
     */
    throttle_s calculate_throttle(const copter_actuation_s& input) const;

private:
    quadcopter_params_s m_params;
    quadcopter_actuators_s m_actuators;
};

}