#pragma once

#include "common/rigidbody.hpp"
#include "common/math.hpp"

namespace mp {

/**
 * Copter actuation
 */
struct copter_actuation_s {
    // Thrust in the upwards direction in Newtons
    float thrust;
    // Torque vector using forward-right-down system in Newton-meters
    vector3f torque;
};

/**
 * Interface for controlling the actuation of a copter-type vehicle.
 * 
 * A "copter" is a vehicle which can generate thrust in a single
 * direction (upwards) and can generate torque in any direction.
 */
class copter {
public:
    /**
     * Calculate and set actuator values to produce the given thrust and torque.
     * @returns `false` if the given arguments are outside of the achievable range,
     * else sets the desired actuation and returns `true`.
     */
    virtual bool actuate(const copter_actuation_s& input) noexcept = 0;

    /**
     * Maximum (absolute) thrust and torque that can be produced.
     */
    virtual copter_actuation_s get_actuation_limit() const noexcept = 0;
};

}