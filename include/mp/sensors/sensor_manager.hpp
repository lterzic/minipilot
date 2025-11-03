#pragma once

#include "accelerometer.hpp"
#include "gyroscope.hpp"
#include "link/telemetry.hpp"

namespace mp {

enum class sensor_type_e {
    ACCELEROMETER,
    GYROSCOPE
};

/**
 * Interface for the system towards sensors available on the platform
 */
class sensor_manager :
    private telemetry_producer<mp_pb_telemetry_Channel_sensors_tag> {
    struct readers_s {
        accelerometer_reader*   accelerometer;
        gyroscope_reader*       gyroscope;
    };
public:
    /**
     * Constructor for creating private reader instances for
     * each provided sensor configuration
     */
    template <
        accelerometer_config_s* accelerometer = nullptr,
        gyroscope_config_s* gyroscope = nullptr
    >
    explicit sensor_manager();

    /**
     * Constructor for externally created reader instances
     */
    explicit sensor_manager(readers_s readers) :
        m_readers(readers)
    {}

    /**
     * Get reader for a specific sensor type
     * @note If sensor is currently unavailable, `nullptr` is returned
     * @todo Once supporting multiple readers of the same
     * type, add index as the parameter and set 0 as default
     */
    template <sensor_type_e SENSOR_TYPE>
    inline const auto* get_sensor_reader() const noexcept;

private:
    /**
     * Generate telemetry data for available sensors
     */
    bool set_telemetry(payload_u& payload) const noexcept override;

private:
    readers_s m_readers;
};

template <
    accelerometer_config_s* accelerometer,
    gyroscope_config_s* gyroscope
>
sensor_manager::sensor_manager()
{
    // TODO: Accelerometer and gyroscope are mandatory so
    // change to references, can't accept nullptr

    if constexpr(accelerometer) {
        static accelerometer_reader reader(*accelerometer);
        m_readers.accelerometer = &reader;
    }

    if constexpr(gyroscope) {
        static gyroscope_reader reader(*gyroscope);
        m_readers.gyroscope = &reader;
    }
}

template <>
inline const auto* sensor_manager::get_sensor_reader<sensor_type_e::ACCELEROMETER>() const noexcept
{
    return m_readers.accelerometer;
}

template <>
inline const auto* sensor_manager::get_sensor_reader<sensor_type_e::GYROSCOPE>() const noexcept
{
    return m_readers.gyroscope;
}

}