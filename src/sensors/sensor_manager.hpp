#pragma once

#include "readers/accelerometer.hpp"
#include "readers/gyroscope.hpp"

namespace mp {

/**
 * Platform sensors
 * 
 * @todo Sensor can be surrounded by #if MP_SENSORS_USE_<SENSOR>
 * to allow conditional compilation and replace with reference
 * instead of pointer
 */
struct sensor_readers_s {
    accelerometer_reader* accelerometer;
    gyroscope_reader* gyroscope;
};

/**
 * Interface for the system towards sensors available on the platform
 */
class sensor_manager {
public:
    explicit sensor_manager(sensor_readers_s readers) :
        m_readers(readers)
    {}

    /**
     * Get reader for a specific sensor type
     * @note If sensor is currently unavailable, `nullptr` is returned
     * @todo Once supporting multiple readers of the same
     * type, add index as the parameter and set 0 as default
     */
    template <typename reader_type>
    const reader_type* get_sensor_reader() const noexcept;

private:
    sensor_readers_s m_readers;
};

template <>
const accelerometer_reader* sensor_manager::get_sensor_reader() const noexcept
{
    return m_readers.accelerometer;
}

template <>
const gyroscope_reader* sensor_manager::get_sensor_reader() const noexcept
{
    return m_readers.gyroscope;
}

}