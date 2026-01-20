#pragma once

#include "common/math.hpp"

namespace mp {

struct rigidbody_params_s {
    // Mass of the aircraft in kilograms
    float mass;
    // Inertia matrix (tensor) - usually a diagonal matrix
    matrix3f moment_of_inertia;
    // Linear drag coefficient
    float lin_drag_c;
    // Angular drag coefficient
    float ang_drag_c;
};

}