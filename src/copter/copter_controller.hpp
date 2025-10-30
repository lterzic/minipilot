#pragma once

#include "common/math.hpp"
#include "state/state.hpp"

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

public:
    /**
     * Run an iteration of the algorithm using angular control input
     */
    virtual copter_actuation_s update_angular(
        const angular_controls_s& input,
        const state_s& state,
        float dt
    ) noexcept = 0;

    /**
     * Run an iteration of the algorithm using linear control input
     */
    virtual copter_actuation_s update_linear(
        const linear_controls_s& input,
        const state_s& state,
        float dt
    ) noexcept = 0;
};

}