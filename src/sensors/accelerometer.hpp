#pragma once

#include "sensor_reader.hpp"
#include "util/math.hpp"
#include <emblib/driver/sensor/accelerometer.hpp>

namespace mp {

/**
 * Data type for meters per second^2 with
 * float as the underlying type
 */
using emblib::mpss_t;

/**
 * Accelerometer sensor interface
 */
using emblib::accelerometer;

/**
 * Accelerometer config
 */
struct accelerometer_config_s {
    // Device reference
    accelerometer& device;
    // Mutex in case the device is part of an IMU with other sensors
    emblib::mutex* mutex;
    // Mapping from the device space into the NED coordinate system
    matrix3f transform;
};

/**
 * Sensor task which provides accelerometer readings
 */
class accelerometer_reader : public sensor_reader<accelerometer::data_t, vector<mpss_t, 3>> {

public:
    explicit accelerometer_reader(const accelerometer_config_s& config);

private:
    bool init(sensor_t& sensor) noexcept override;

    vector<mpss_t, 3> process(const accelerometer::data_t& raw_data) noexcept override;

private:
    // Transform maps a potentially different coordinate space
    // of the readings into the NED coordinate system
    matrix<float, 3> m_transform;
    // This can be calculated or read from the configuration
    // depending on if the vehicle is on stable ground before
    // starting
    vector<mpss_t, 3> m_bias;

    // TODO: Add low-pass filter
};

}