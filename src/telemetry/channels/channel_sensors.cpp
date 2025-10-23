#include "telemetry/channels/channel_sensors.hpp"
#include "common/pb.hpp"

namespace mp {

channel_sensors::channel_sensors(
    telemetry& telemetry,
    const sensor_manager& sensor_manager,
    const state_estimator_task& state_estimator_task
) :
    m_sensor_manager(sensor_manager),
    m_state_estimator_task(state_estimator_task)
{
    assert(telemetry.add_channel(
        mp_pb_telemetry_Channel_sensors_tag,
        etl::make_delegate<channel_sensors, &channel_sensors::set>(*this)
    ));
}

void
channel_sensors::set(telemetry::channel_payload_u& channel) const noexcept
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
        PB_SET(channel.sensors.gyroscope, drift, m_state_estimator_task.get_state().gyroscope_drift);
        channel.sensors.has_gyroscope = true;
    }
}
    
}