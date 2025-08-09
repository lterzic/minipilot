#pragma once

#include "sensor_reader.hpp"
#include "util/math.hpp"
#include <emblib/driver/sensor/gyroscope.hpp>

namespace mp {

/**
 * Gyroscope config
 */
struct gyroscope_config_s {
    // Device reference
    emblib::gyroscope& device;
    // This mutex (if provided) is locked during reads
    // Used when there are multiple sensors on one device
    emblib::mutex* mutex;
    // Mapping from the device space into the NED coordinate system
    matrix3f transform;
};

/**
 * Sensor task which provides gyroscope readings
 */
class gyroscope_reader : public sensor_reader<
    emblib::gyroscope::data_t,
    vector<emblib::rps_t, 3>
> {
public:
    explicit gyroscope_reader(const gyroscope_config_s& config);

private:
    bool init(sensor_t& sensor) noexcept override;

    vector<emblib::rps_t, 3> process(const sensor_t::data_t& raw_data) noexcept override;

private:
    // Transform maps a potentially different coordinate space
    // of the readings into the NED coordinate system
    matrix<float, 3> m_transform;
    // Currently estimated drift
    vector<emblib::rps_t, 3> m_drift;
    // TODO: Add a mutex and a getter method for the drift

    // TODO: Add band-pass filter
};

}