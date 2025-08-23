#include "telemetry.hpp"
#include "mp_config.hpp"

#define PB_SET(struct, field, value) pb_set(struct.field, struct.has_ ## field, value)

namespace mp {

static void pb_set(mp_pb_Vector3f& pb_vec, bool& pb_has_field, const vector3f& mp_vec)
{
    pb_vec.x = mp_vec(0);
    pb_vec.y = mp_vec(1);
    pb_vec.z = mp_vec(2);
    pb_has_field = true;
}

template <typename unit_type, size_t SIZE>
static void pb_set(mp_pb_Vector3f& pb_vec, bool& pb_has_field, const etl::array<unit_type, SIZE>& array)
{
    pb_vec.x = array[0].value();
    pb_vec.y = array[1].value();
    pb_vec.z = array[2].value();
    pb_has_field = true;
}

static void pb_set(mp_pb_Vector4f& pb_vec, bool& pb_has_field, const vector4f& mp_vec)
{
    pb_vec.w = mp_vec(0);
    pb_vec.x = mp_vec(1);
    pb_vec.y = mp_vec(2);
    pb_vec.z = mp_vec(3);
    pb_has_field = true;
}

telemetry::telemetry(
    tx& tx,
    const sensor_manager& sensor_manager,
    const state_estimator& state_estimator
) :
    static_task("Telemetry", TELEMETRY_TASK_PRIORITY),
    m_tx(tx),
    m_sensor_manager(sensor_manager),
    m_state_estimator(state_estimator)
{}

void telemetry::run() noexcept
{
    while (true) {
        m_tx.send_downlink([this](auto& payload, auto& payload_type) {
            payload_type = mp_pb_link_DownlinkMessage_telemetry_tag;
            mp_pb_link_Telemetry& msg = payload.telemetry;

            msg.timestamp_ms = get_time_since_start().value();
            
            state_s state = m_state_estimator.get_state();
            PB_SET(msg.state, position, state.position);
            PB_SET(msg.state, velocity, state.velocity);
            PB_SET(msg.state, acceleration, state.acceleration);
            PB_SET(msg.state, angular_velocity, state.angular_velocity);
            PB_SET(msg.state, rotation, state.rotationq.as_vector());
            msg.has_state = true;

            auto accelerometer = m_sensor_manager.get_sensor_reader<sensor_type_e::ACCELEROMETER>();
            PB_SET(msg.sensors.accelerometer, raw, accelerometer->get_raw());
            PB_SET(msg.sensors.accelerometer, corrected, accelerometer->get_processed().cast<float>());
            msg.sensors.has_accelerometer = true;
            
            auto gyroscope = m_sensor_manager.get_sensor_reader<sensor_type_e::GYROSCOPE>();
            PB_SET(msg.sensors.gyroscope, raw, gyroscope->get_raw());
            PB_SET(msg.sensors.gyroscope, corrected, gyroscope->get_processed().cast<float>());
            PB_SET(msg.sensors.gyroscope, drift, state.gyroscope_drift);
            msg.sensors.has_gyroscope = true;

            msg.has_sensors = true;
        });

        sleep_periodic(TELEMETRY_PERIOD);
    }
}

}