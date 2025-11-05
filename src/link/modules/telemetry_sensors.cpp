#include "link/modules/telemetry_sensors.hpp"
#include "common/pb.hpp"

namespace mp {

telemetry_sensors::telemetry_sensors(
    telemetry& telemetry,
    const sensor_manager& sensor_manager,
    const state_estimator_task& estimator
) :
    m_sensor_manager(sensor_manager),
    m_estimator(estimator)
{
    assert(telemetry.add_producer(mp_pb_telemetry_Channel_sensors_tag, *this));
}

bool
telemetry_sensors::produce(payload_u& payload) const noexcept
{
    auto accelerometer = m_sensor_manager.get_sensor_reader<sensor_type_e::ACCELEROMETER>();
    if (accelerometer) {
        PB_SET(payload.sensors.accelerometer, raw, accelerometer->get_raw());
        PB_SET(payload.sensors.accelerometer, corrected, accelerometer->get_processed().cast<float>());
        payload.sensors.has_accelerometer = true;
    }

    auto gyroscope = m_sensor_manager.get_sensor_reader<sensor_type_e::GYROSCOPE>();
    if (gyroscope) {
        PB_SET(payload.sensors.gyroscope, raw, gyroscope->get_raw());
        PB_SET(payload.sensors.gyroscope, corrected, gyroscope->get_processed().cast<float>());
        PB_SET(payload.sensors.gyroscope, drift, m_estimator.get_state().gyroscope_drift);
        payload.sensors.has_gyroscope = true;
    }

    return true;
}

}