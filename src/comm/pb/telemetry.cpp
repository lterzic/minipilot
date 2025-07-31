#include "telemetry.hpp"
#include "mp_config.hpp"
#include "util/pb_util.hpp"

namespace mp {

telemetry::telemetry(
    pb_tx& tx,
    const sensor_manager& sensor_manager,
    const state_estimator& state_estimator
) :
    task("Telemetry", TELEMETRY_TASK_PRIORITY, m_stack),
    m_tx(tx),
    m_sensor_manager(sensor_manager),
    m_state_estimator(state_estimator)
{}

void telemetry::run() noexcept
{
    while (true) {
        m_tx.send_downlink([this](auto& payload, auto& payload_type) {
            payload_type = pb_mp_DownlinkMessage_telemetry_tag;
            pb_mp_Telemetry& msg = payload.telemetry;

            msg.timestamp_ms = get_time_since_start().value();
            
            state_s state = m_state_estimator.get_state();
            PB_SET(msg.state, position, state.position);
            PB_SET(msg.state, velocity, state.velocity);
            PB_SET(msg.state, acceleration, state.acceleration);
            PB_SET(msg.state, angular_velocity, state.angular_velocity);
            PB_SET(msg.state, rotation, state.rotationq.as_vector());
            msg.has_state = true;

            auto accelerometer = m_sensor_manager.get_sensor_reader<accelerometer_reader>();
            auto gyroscope = m_sensor_manager.get_sensor_reader<gyroscope_reader>();
            PB_SET(msg.sensors.accelerometer, corrected, accelerometer->get_processed().cast<float>());
            PB_SET(msg.sensors.gyroscope, corrected, gyroscope->get_processed().cast<float>());
            msg.sensors.has_accelerometer = true;
            msg.sensors.has_gyroscope = true;
            msg.has_sensors = true;
        });

        sleep_periodic(TELEMETRY_PERIOD);
    }
}

}