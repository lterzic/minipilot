#pragma once

#include "common/math.hpp"
#include <emblib/rtos/mutex.hpp>
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

public:
    /**
     * By default initialize input to angular mode with 0 thrust and angular velocity
     */
    copter_controller() noexcept;

    /**
     * Set the desired angular velocity vector and thrust
     * @returns `false` if the given angular velocity or thrust can't be achieved
     */
    bool set_angular_controls(angular_controls_s input) noexcept;

    /**
     * Set the desired velocity vector and direction
     * @returns `false` if the given velocity can't be achieved
     */
    bool set_linear_controls(linear_controls_s input) noexcept;

    /**
     * Get the required actuation to achieve the last set command
     */
    virtual actuation_s iterate(float dt) noexcept = 0;

protected:
    /**
     * Current control mode can be either angular or linear
     * @note Index 0 represents angular controls, and index 1 linear
     */
    using control_v = etl::variant<angular_controls_s, linear_controls_s>;

    /**
     * Get current controls
     */
    control_v get_controls() const noexcept
    {
        emblib::rtos::scoped_lock lock(m_mutex);
        return m_controls;
    }

private:
    // For protecting controls while writing
    mutable emblib::rtos::mutex m_mutex;
    // Current controls
    control_v m_controls;
};

}