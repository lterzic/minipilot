#pragma once

#include "task_config.hpp"
#include "task_sensor.hpp"
#include "util/math.hpp"
#include <emblib/driver/sensor/accelerometer.hpp>

namespace mp {

/**
 * Data type for meters per second^2 with
 * float as the underlying type
 */
using emblib::mpss_t;

/**
 * Sensor task which provides accelerometer readings
 */
class task_accelerometer : public task_sensor<vector<mpss_t, 3>> {

public:
    explicit task_accelerometer(
        emblib::accelerometer<mpss_t>& accelerometer,
        matrix<float, 3> transform
    ) : task_sensor(
        accelerometer,
        "Task accelerometer",
        TASK_ACCEL_PRIORITY,
        TASK_ACCEL_PERIOD
    ), m_transform(transform) {}

private:
    bool init(emblib::sensor<vector<mpss_t, 3>>& sensor) noexcept override;

    vector<mpss_t, 3> process(const vector<mpss_t, 3>& raw_data) const noexcept override;

private:
    // Transform maps a potentially different coordinate space
    // of the readings into the minipilot coordinate system and
    // addresses any misalignments
    matrix<float, 3> m_transform;
    vector<mpss_t, 3> m_bias;
    // TODO: Add low-pass filter
};

}