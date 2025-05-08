#pragma once

#include "task_config.hpp"
#include "task_sensor.hpp"
#include "util/math.hpp"
#include <emblib/driver/sensor/gyroscope.hpp>

namespace mp {

/**
 * Data type for radians per second with
 * float as the underlying type
 */
using emblib::rps_t;

/**
 * Sensor task which provides gyroscope readings
 * @note Data type is a 3-dim vector of radians per second
 */
class task_gyroscope : public task_sensor<vector<rps_t, 3>> {

public:
    explicit task_gyroscope(
        emblib::gyroscope<rps_t>& gyroscope,
        matrix<float, 3> transform
    ) : task_sensor(
        gyroscope,
        "Task gyroscope",
        TASK_GYRO_PRIORITY,
        TASK_GYRO_PERIOD
    ), m_transform(transform) {}

private:
    bool init(emblib::sensor<vector<rps_t, 3>>& sensor) noexcept override;
    
    vector<rps_t, 3> process(const vector<rps_t, 3>& raw_data) const noexcept override;

private:
    // Transform maps a potentially different coordinate space
    // of the readings into the minipilot coordinate system and
    // addresses any misalignments
    matrix<float, 3> m_transform;
    // TODO: Add band-pass filter
};

}