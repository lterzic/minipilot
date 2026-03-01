#pragma once

#include <mp/algo/math/vector.hpp>
#include <mp/algo/math/quaternion.hpp>

namespace mp::algo {

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

}