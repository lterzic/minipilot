#include "sensors/sensor_manager.hpp"
#include "common/pb.hpp"

namespace mp {

bool
sensor_manager::set_telemetry(payload_u& payload) const noexcept
{
    auto accelerometer = get_sensor_reader<sensor_type_e::ACCELEROMETER>();
    if (accelerometer) {
        PB_SET(payload.sensors.accelerometer, raw, accelerometer->get_raw());
        PB_SET(payload.sensors.accelerometer, corrected, accelerometer->get_processed().cast<float>());
        payload.sensors.has_accelerometer = true;
    }

    auto gyroscope = get_sensor_reader<sensor_type_e::GYROSCOPE>();
    if (gyroscope) {
        PB_SET(payload.sensors.gyroscope, raw, gyroscope->get_raw());
        PB_SET(payload.sensors.gyroscope, corrected, gyroscope->get_processed().cast<float>());
        // PB_SET(payload.sensors.gyroscope, drift, m_state_estimator_task.get_state().gyroscope_drift);
        payload.sensors.has_gyroscope = true;
    }

    return true;
}

}