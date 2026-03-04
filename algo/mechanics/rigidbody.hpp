#pragma once

#include "kinematics.hpp"
#include "mp/algo/math/matrix.hpp"

namespace mp::algo {

struct rigidbody_s {
    // Mass of the body in kilograms
    const float mass;
    // Moment of inertia matrix in kg*m^2
    const matrix3f moment_of_inertia;
    // Kinematics of the center of mass (CoM)
    kinematics_s kinematics;

    /**
     * Convert the given external force and torque into accelerations
     * and update the kinematics of the rigidbody.
     */
    void update(float dt, const vector3f& force, const vector3f& torque) noexcept;
};

}