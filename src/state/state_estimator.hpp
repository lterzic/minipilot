#pragma once

#include "util/math.hpp"

namespace mp {

/**
 * State of the system
 * 
 * This is generated by the state estimator task and used
 * by the rest of the system to get current information about
 * the vehicle
 * 
 * State variables also shortened to:
 * `p` - position
 * `v` - (linear) velocity
 * `a` - (linear) acceleration
 * `w` (omega) - angular velocity
 * `q` - rotation quaternion
 * 
 * @note This is a public interface since this can also be provided
 * to external systems through the external api task
 */
struct state_s {
    // Position of the center of mass (CoM) of the vehicle in
    // the global (inertial) reference frame
    vector3f position {0, 0, 0};
    // (Linear) velocity of the CoM (= d/dt(position))
    vector3f velocity {0, 0, 0};
    // (Linear) acceleration of the CoM in the global reference frame
    vector3f acceleration {0, 0, 0};
    // Angular acceleration vector in the local rotating (non-inertial) reference frame
    vector3f angular_velocity {0, 0, 0};
    // Quaternion which maps the local frame to the global frame
    quaternionf rotationq {1, 0, 0, 0};
};

/**
 * Input for a state estimator
 * @note Assign `nullptr` if the appropriate value
 * is not available
 */
struct sensor_data_s {
    const vector3f* accelerometer = nullptr;
    const matrix3f* accelerometer_cov = nullptr;

    const vector3f* gyroscope = nullptr;
    const matrix3f* gyroscope_cov = nullptr;

    const vector3f* magnetometer = nullptr;
    const matrix3f* magnetometer_cov = nullptr;
    
    const vector3f* gnss = nullptr;
    const matrix3f* gnss_cov = nullptr;
};

/**
 * State estimation algorithm interface
 */
class state_estimator {

public:
    /**
     * Algorithm iteration
     */
    virtual void update(const sensor_data_s& input, float dt) noexcept = 0;

    /**
     * Get the current state
     */
    virtual state_s get_state() const noexcept = 0;
};

}