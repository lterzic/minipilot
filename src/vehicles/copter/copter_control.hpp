#pragma once

#include "util/math.hpp"

namespace mp {

/**
 * Interface for controlling a `copter`
 */
class copter_control {
public:
    struct actuation_s {
        // Thrust in Newtons in the "UP" direction
        float thrust;
        // Torque in the forward-right-down system in Newton-meters
        vector3f torque;
    };

public:
    /**
     * Set the desired angular velocity vector and thrust
     * @param velocity Angular velocity vector in forward-right-down system in radians per second
     * @param thrust Thrust in the "UP" direction in Newtons
     * @returns `false` if the given angular velocity or thrust can't be achieved
     */
    virtual bool set_angular_velocity(vector3f velocity, float thrust) noexcept = 0;

    /**
     * Set the desired velocity vector and direction
     * @param velocity Velocity vector in the global NED coordinate system in meters per second
     * @param dir Direction in radians with 0 being North, going clockwise
     * @returns `false` if the given velocity can't be achieved
     */
    virtual bool set_linear_velocity(vector3f velocity, float dir) noexcept = 0;

    /**
     * Get the required actuation to achieve the last set command
     */
    virtual actuation_s get_actuation() const noexcept = 0;

};

}