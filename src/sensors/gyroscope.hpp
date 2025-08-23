#pragma once

#include "sensor_reader.hpp"
#include "util/math.hpp"
#include <emblib/devices/sensors/gyroscope.hpp>

namespace mp {

/**
 * Gyroscope config
 */
struct gyroscope_config_s {
    // Device reference
    emblib::devices::gyroscope& device;
    // This mutex (if provided) is locked during reads
    // Used when there are multiple sensors on one device
    emblib::rtos::mutex* mutex;
    // Mapping from the device space into the NED coordinate system
    matrix3f transform;
};

/**
 * Output units of a gyroscope
 */
using gyroscope_units = emblib::devices::gyroscope_units;

/**
 * Sensor task which provides gyroscope readings
 */
class gyroscope_reader : public sensor_reader<
    emblib::devices::gyroscope::data_t,
    vector<gyroscope_units, 3>
> {
public:
    explicit gyroscope_reader(const gyroscope_config_s& config);

private:
    bool init(sensor_t& sensor) noexcept override;

    vector<gyroscope_units, 3> process(const sensor_t::data_t& raw_data) noexcept override;

private:
    // Transform maps a potentially different coordinate space
    // of the readings into the NED coordinate system
    matrix<float, 3> m_transform;

    // TODO: Add band-pass filter
};

}