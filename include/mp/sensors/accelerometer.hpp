#pragma once

#include "common/math.hpp"
#include "sensors/sensor_reader.hpp"
#include <emblib/devices/sensors/accelerometer.hpp>

namespace mp {

/**
 * Accelerometer config
 */
struct accelerometer_config_s {
    // Device reference
    emblib::devices::accelerometer& device;
    // This mutex (if provided) is locked during reads
    // Used when there are multiple sensors on one device
    emblib::rtos::mutex* mutex;
    // Mapping from the device space into the NED coordinate system
    matrix3f transform;
};

/**
 * Output units of an accelerometer
 */
using accelerometer_units = emblib::devices::accelerometer_units;

/**
 * Sensor task which provides accelerometer readings
 */
class accelerometer_reader : public sensor_reader<
    emblib::devices::accelerometer::data_t,
    vector<accelerometer_units, 3>
> {
public:
    explicit accelerometer_reader(const accelerometer_config_s& config);

private:
    bool init(sensor_t& sensor) noexcept override;

    vector<accelerometer_units, 3> process(const sensor_t::data_t& raw_data) noexcept override;

private:
    // Transform maps a potentially different coordinate space
    // of the readings into the NED coordinate system
    matrix<float, 3> m_transform;
    // This can be calculated or read from the configuration
    // depending on if the vehicle is on stable ground before
    // starting
    vector<accelerometer_units, 3> m_bias;

    // TODO: Add low-pass filter
};

}