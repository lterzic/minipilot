#pragma once

#include "common/math.hpp"

namespace mp {

/**
 * Parameters of a generic copter model
 */
struct copter_params_s {
    // Mass of the aircraft in kilograms
    float mass;
    // Inertia matrix (tensor) - usually a diagonal matrix
    matrix3f moment_of_inertia;
    // Linear drag coefficient
    float lin_drag_c;
    // Angular drag coefficient
    float ang_drag_c;
};

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
 * direction (usually "UP") and can generate torque in any direction.
 * 
 * @todo Extend `model`
 */
class copter {

public:
    explicit copter(copter_params_s& params) noexcept :
        m_params(params)
    {}

    /**
     * Calculate and set actuator values to produce the given thrust and torque
     * @returns `false` if the given arguments are outside of the achievable range,
     * else sets the desired actuation and returns `true`
     */
    virtual bool actuate(const copter_actuation_s& input) noexcept = 0;

private:
    // Parameters of this vehicle
    copter_params_s& m_params;
};

}