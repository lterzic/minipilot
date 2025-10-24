#pragma once

#include "common/math.hpp"
#include "state/state.hpp"
#include <etl/variant.h>

namespace mp {

/**
 * Interface for controlling a `copter`
 */
class copter_controller {
public:
    struct actuation_s {
        // Thrust in Newtons in the "UP" direction
        float thrust;
        // Torque in the forward-right-down system in Newton-meters
        vector3f torque;
    };

    struct angular_controls_s {
        // Angular velocity vector in radians per second
        vector3f velocity;
        // Thrust in the "UP" direction in Newtons
        float thrust;
    };

    struct linear_controls_s {
        // Velocity vector in meters per second
        vector3f velocity;
        // Direction in radians with 0 being North, increasing clockwise
        float direction;
    };

    /**
     * Current control mode can be either angular or linear
     * @note Index 0 represents angular controls, and index 1 linear
     */
    using control_v = etl::variant<angular_controls_s, linear_controls_s>;

public:
    /**
     * Compute the required actuation to achieve the desired control input
     * based on the current vehicle state
     */
    virtual actuation_s update(float dt, const control_v& controls, const state_s& state) noexcept = 0;
};

}