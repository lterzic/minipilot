#include "task_telemetry.hpp"
#include "util/pb_util.hpp"
#include "pb/mp/telemetry.nanopb.h"
#include <pb_encode.h>

namespace mp {

task_telemetry::task_telemetry(
    emblib::char_dev& telemetry_device,
    task_accelerometer& task_accelerometer,
    task_gyroscope& task_gyroscope,
    task_state_estimator& task_state_estimator
) :
    task("Task telemetry", TASK_TELEMETRY_PRIORITY, m_task_stack),
    m_telemetry_device(telemetry_device),
    m_task_accel(task_accelerometer),
    m_task_gyro(task_gyroscope),
    m_task_state(task_state_estimator)
{}

void task_telemetry::run() noexcept
{
    // This doesn't have to be an assert
    // Can just exit and turn off the telemetry task
    assert(m_telemetry_device.probe(emblib::milliseconds_t(0)));

    while (true) {
        pb_mp_TelemetryMessage msg = pb_mp_TelemetryMessage_init_zero;

        // State data
        state_s state = m_task_state.get_state();
        pb_vector3f_set(msg.state.position, state.position);
        pb_vector3f_set(msg.state.velocity, state.velocity);
        pb_vector3f_set(msg.state.acceleration, state.acceleration);
        pb_vector3f_set(msg.state.angular_velocity, state.angular_velocity);
        pb_vector4f_set(msg.state.rotation, state.rotationq.as_vector());
        msg.state.has_position = true;
        msg.state.has_velocity = true;
        msg.state.has_acceleration = true;
        msg.state.has_angular_velocity = true;
        msg.state.has_rotation = true;
        msg.has_state = true;
        
        // Sensor data
        pb_vector3f_set(msg.sensor_data.acc_raw, m_task_accel.get_raw().cast<float>());
        pb_vector3f_set(msg.sensor_data.acc_corrected, m_task_accel.get_corrected().cast<float>());
        pb_vector3f_set(msg.sensor_data.gyro_raw, m_task_gyro.get_raw().cast<float>());
        pb_vector3f_set(msg.sensor_data.gyro_corrected, m_task_gyro.get_corrected().cast<float>());
        msg.sensor_data.has_acc_raw = true;
        msg.sensor_data.has_acc_corrected = true;
        msg.sensor_data.has_gyro_raw = true;
        msg.sensor_data.has_gyro_corrected = true;
        msg.has_sensor_data = true;

        // TODO: Send vehicle specific telemetry here

        // Messages are encoded into a buffer before being sent
        // to avoid message fragmentation since the output device
        // can be used by other tasks such as the logger
        char out_buffer[sizeof(pb_mp_TelemetryMessage)];
        pb_ostream_t pb_ostream = pb_ostream_from_buffer((pb_byte_t*)out_buffer, sizeof(out_buffer));
        
        if (pb_encode(&pb_ostream, pb_mp_TelemetryMessage_fields, &msg)) {
            if (m_telemetry_device.is_async_available()) {
                bool start_status = m_telemetry_device.write_async(out_buffer, pb_ostream.bytes_written, [this](ssize_t status) {
                    notify_from_isr();
                });

                if (start_status)
                    wait_notification(emblib::MILLISECONDS_MAX);
            } else {
                m_telemetry_device.write(out_buffer, pb_ostream.bytes_written, std::chrono::milliseconds(0));
            }
        } else {
            log_error("Failed to encode telemetry!");
        }
        
        sleep_periodic(TASK_TELEMETRY_PERIOD);
    }
}

}