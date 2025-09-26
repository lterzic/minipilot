#include "sensors/sensor_telemetry.hpp"
#include "common/pb.hpp"

namespace mp {

sensor_telemetry::sensor_telemetry(
    const sensor_manager& sensor_manager,
    const state_estimator& state_estimator,
    telemetry& telemetry
) :
    m_sensor_manager(sensor_manager),
    m_state_estimator(state_estimator),
    m_telemetry(telemetry)
{
    assert(m_telemetry.add_channel(
        mp_pb_telemetry_Channel_sensors_tag,
        etl::make_delegate<sensor_telemetry, &sensor_telemetry::channel_sensors>(*this)
    ));
}

void
sensor_telemetry::channel_sensors(telemetry::channel_payload_u& channel) const noexcept
{
    auto accelerometer = m_sensor_manager.get_sensor_reader<sensor_type_e::ACCELEROMETER>();
    if (accelerometer) {
        PB_SET(channel.sensors.accelerometer, raw, accelerometer->get_raw());
        PB_SET(channel.sensors.accelerometer, corrected, accelerometer->get_processed().cast<float>());
        channel.sensors.has_accelerometer = true;
    }

    auto gyroscope = m_sensor_manager.get_sensor_reader<sensor_type_e::GYROSCOPE>();
    if (gyroscope) {
        PB_SET(channel.sensors.gyroscope, raw, gyroscope->get_raw());
        PB_SET(channel.sensors.gyroscope, corrected, gyroscope->get_processed().cast<float>());
        PB_SET(channel.sensors.gyroscope, drift, m_state_estimator.get_state().gyroscope_drift);
        channel.sensors.has_gyroscope = true;
    }
}
    
}