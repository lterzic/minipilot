#pragma once

namespace mp {

/**
 * Common vehicle interface
 * @todo inherit from this in copter, plane, ...
 */
class vehicle {
public:
    /**
     * Vehicle name
     */
    virtual const char* get_name() const noexcept = 0;

    /**
     * Allow the vehicle to control the actuators
     */
    virtual bool arm() noexcept = 0;

    /**
     * Forbid the vehicle to control the actuators
     */
    virtual bool disarm() noexcept = 0;
};

}