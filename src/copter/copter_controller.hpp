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
    struct angular_controls_s {
        // Angular velocity vector in radians per second
        vector3f angular_velocity;
        // Thrust in the "UP" direction in Newtons
        float thrust;
    };

    struct linear_controls_s {
        // Velocity vector in meters per second
        vector3f linear_velocity;
        // Direction in radians with 0 being North, increasing clockwise
        float direction;
    };

    /**
     * Copter can be controlled using either mode
     */
    using controls_v = etl::variant<angular_controls_s, linear_controls_s>;

public:
    /**
     * Run an iteration of the algorithm using angular control input
     */
    virtual copter_actuation_s update(
        const controls_v& input,
        const state_s& state,
        float dt
    ) noexcept = 0;

};

}