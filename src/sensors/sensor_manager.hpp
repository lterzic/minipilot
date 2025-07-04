#pragma once

#include "accelerometer.hpp"
#include "gyroscope.hpp"

namespace mp {

/**
 * Platform sensors
 * 
 * @todo Sensor can be surrounded by #if MP_SENSORS_USE_<SENSOR>
 * to allow conditional compilation and replace with reference
 * instead of pointer
 */
struct sensors_s {
    accelerometer_reader* accelerometer;
    gyroscope_reader* gyroscope;
};

/**
 * Interface for the system towards sensors available on the platform
 */
class sensor_manager {
public:
    explicit sensor_manager(sensors_s sensors) :
        m_sensors(sensors)
    {}

    /**
     * Get reader for a specific sensor type
     * @note If sensor is currently unavailable, `nullptr` is returned
     * @todo Once supporting multiple readers of the same
     * type, add index as the parameter and set 0 as default
     */
    template <typename reader_type>
    inline const reader_type* get_sensor_reader() const noexcept;

private:
    sensors_s m_sensors;
};

template <>
inline const accelerometer_reader* sensor_manager::get_sensor_reader() const noexcept
{
    return m_sensors.accelerometer;
}

template <>
inline const gyroscope_reader* sensor_manager::get_sensor_reader() const noexcept
{
    return m_sensors.gyroscope;
}

}