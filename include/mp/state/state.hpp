#pragma once

#include "common/math.hpp"

namespace mp {

/**
 * Kinematics of a fixed point on the vehicle, usually CoM
 */
struct kinematics_s {
    // Position in an intertial frame relative to the starting point in meters
    vector3f position {0, 0, 0};
    // (Linear) velocity of the CoM (= d/dt(position))
    vector3f velocity {0, 0, 0};
    // (Linear) acceleration of the CoM in the global reference frame
    vector3f acceleration {0, 0, 0};
    // Quaternion which maps the local frame to the global frame
    quaternionf rotation {1, 0, 0, 0};
    // Angular acceleration vector in the local rotating (non-inertial) reference frame
    vector3f angular_velocity {0, 0, 0};
};

/**
 * State of the vehicle consists of kinematics of the center
 * of mass of the vehicle, whether the vehicle is on ground,
 * gyroscope drift, ...
 * @note This struct should match with the `State` message
 * in `state.proto`
 */
struct state_s {
    // Kinematics of center of mass
    kinematics_s kinematics;
    // Gyroscope drift in radians per second
    vector3f gyroscope_drift;
    // Whether there is a normal force from the ground
    // preventing acceleration in gravity direction
    bool grounded;
};

}