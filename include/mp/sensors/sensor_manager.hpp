#pragma once

#include "sensors/accelerometer.hpp"
#include "sensors/gyroscope.hpp"
#include <etl/optional.h>

namespace mp {

/**
 * Sensor type enumeration used to allow compile time sensor
 * type selection
 */
enum class sensor_type_e {
    ACCELEROMETER,
    GYROSCOPE
};

/**
 * Sensor manager provides only access to sensors available
 * on the system
 */
class sensor_manager {
    /**
     * Sensor type aggregation for convenience
     * @note `nullptr` means that the sensor type is not available
     */
    struct sensors_s {
        accelerometer* m_accelerometer;
        gyroscope* m_gyroscope;
    };

public:
    /**
     * Initialize the sensor manager with all available sensors on the system
     */
    explicit sensor_manager(const sensors_s& sensors);

    /**
     * Get the sensor of a specific type
     * @note If sensor is currently unavailable, `nullptr` is returned
     * @todo Once supporting multiple sensors of the same
     * type, add index as the parameter and set 0 as default
     */
    template <sensor_type_e SENSOR_TYPE>
    inline const auto& get_sensor() const noexcept;

private:
    etl::optional<accelerometer_processor> m_accelerometer;
    etl::optional<gyroscope_processor> m_gyroscope;
};

template <>
inline const auto& sensor_manager::get_sensor<sensor_type_e::ACCELEROMETER>() const noexcept
{
    return m_accelerometer;
}

template <>
inline const auto& sensor_manager::get_sensor<sensor_type_e::GYROSCOPE>() const noexcept
{
    return m_gyroscope;
}

}