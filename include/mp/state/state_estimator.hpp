#pragma once

#include "state/state.hpp"
#include <etl/optional.h>

namespace mp {

/**
 * State estimation algorithm interface
 */
class state_estimator {
public:
    struct sensors_s {
        // Accelerometer data in m/s^2, noise density in
        etl::optional<etl::pair<vector3f, float>> accelerometer;
        // Gyroscope data in rad/s, noise density in 
        etl::optional<etl::pair<vector3f, float>> gyroscope;
    };

public:
    /**
     * Run an iteration of the algorithm and return the new
     * state estimation
     */
    virtual state_s update(float dt, const sensors_s& sensors) noexcept = 0;
};

}