#pragma once

#include "state.hpp"
#include <etl/optional.h>

namespace mp {

/**
 * State estimation algorithm interface
 */
class estimator {
public:
    /**
     * Data which is new since the last iteration of the algorithm
     * @note For each sensor there is a pair (data, noise density)
     * where noise density has units of [data]/sqrt(Hz)
     */
    struct sensor_data_s {
        // Accelerometer data in m/s^2
        etl::optional<etl::pair<vector3f, float>> accelerometer;
        // Gyroscope data in rad/s
        etl::optional<etl::pair<vector3f, float>> gyroscope;
    };

public:
    /**
     * Run an iteration of the algorithm and return the new
     * state estimation
     */
    virtual state_s update(float dt, const sensor_data_s& sensor_data) noexcept = 0;
};

}