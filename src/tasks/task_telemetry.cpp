#include "task_telemetry.hpp"
#include "util/chrono.hpp"
#include "util/pb_util.hpp"
#include "pb/mp/telemetry.nanopb.h"
#include <pb_encode.h>

namespace mp {

task_telemetry::task_telemetry(
    emblib::io::ostream<char>& telemetry_device,
    task_accelerometer& task_accelerometer,
    task_gyroscope& task_gyroscope,
    task_state_estimator& task_state_estimator
) :
    task("Task telemetry", TASK_TELEMETRY_PRIORITY, m_task_stack),
    m_telemetry_device(telemetry_device),
    m_task_accel(task_accelerometer),
    m_task_gyro(task_gyroscope),
    m_task_state(task_state_estimator),
    m_msg_id(0)
{}

void task_telemetry::run() noexcept
{
    // TODO: Add a health check call for the output device

    while (true) {
        pb_mp_Telemetry msg = pb_mp_Telemetry_init_zero;

        // Set the timestamp to the current time relative to the start
        // When the first telemetry messages is received this is used
        // to set the session start time and all other messages
        // are timestamped relative to that time
        msg.timestamp_ms = get_time_since_start().value();

        // Assign the counter for the current session and increment
        msg.message_id = m_msg_id;
        m_msg_id++;

        // State data
        state_s state = m_task_state.get_state();
        PB_SET(msg.state, position, state.position);
        PB_SET(msg.state, velocity, state.velocity);
        PB_SET(msg.state, acceleration, state.acceleration);
        PB_SET(msg.state, angular_velocity, state.angular_velocity);
        PB_SET(msg.state, rotation, state.rotationq.as_vector());
        msg.has_state = true;
        
        // Sensor data
        PB_SET(msg.sensors.accelerometer, raw, m_task_accel.get_raw().cast<float>());
        PB_SET(msg.sensors.accelerometer, corrected, m_task_accel.get_corrected().cast<float>());
        PB_SET(msg.sensors.gyroscope, raw, m_task_gyro.get_raw().cast<float>());
        PB_SET(msg.sensors.gyroscope, corrected, m_task_gyro.get_corrected().cast<float>());
        msg.sensors.has_accelerometer = true;
        msg.sensors.has_gyroscope = true;
        msg.has_sensors = true;

        // TODO: Send vehicle specific telemetry here

        // Messages are encoded into a buffer before being sent
        // to avoid message fragmentation since the output device
        // can be used by other tasks such as the logger
        char out_buffer[pb_mp_Telemetry_size];
        pb_ostream_t pb_ostream = pb_ostream_from_buffer((pb_byte_t*)out_buffer, sizeof(out_buffer));
        
        if (pb_encode(&pb_ostream, pb_mp_Telemetry_fields, &msg)) {
            if (m_telemetry_device.is_async_available()) {
                bool start_status = m_telemetry_device.write_async(out_buffer, pb_ostream.bytes_written, [this](ssize_t status) {
                    notify_from_isr();
                });

                if (start_status)
                    wait_notification(emblib::MILLISECONDS_MAX);
            } else {
                m_telemetry_device.write(out_buffer, pb_ostream.bytes_written, milliseconds_t(0));
            }
        } else {
            log_error("Failed to encode telemetry!");
        }
        
        sleep_periodic(TASK_TELEMETRY_PERIOD);
    }
}

}