#include "state/state_estimator_task.hpp"
#include "common/pb.hpp"

namespace mp {

void
state_estimator_task::run() noexcept
{
    float dt = emblib::units::seconds<float>(m_period).value();

    size_t acc_count = 0;
    size_t gyro_count = 0;
    while (true) {
        state_estimator::sensors_s sensors;
        
        auto acc = m_sensor_manager.get_sensor_reader<sensor_type_e::ACCELEROMETER>();
        if (acc) {
            size_t curr_count = acc->get_sample_count();
            if (curr_count > acc_count) {
                acc_count = curr_count;
                sensors.accelerometer = {
                    acc->get_processed().cast<float>(),
                    acc->get_sensor().get_noise_density()
                };
            }
        }

        auto gyro = m_sensor_manager.get_sensor_reader<sensor_type_e::GYROSCOPE>();
        if (gyro) {
            size_t curr_count = gyro->get_sample_count();
            if (curr_count > gyro_count) {
                gyro_count = curr_count;
                sensors.gyroscope = {
                    gyro->get_processed().cast<float>(),
                    gyro->get_sensor().get_noise_density()
                };
            }
        }

        auto state = m_estimator.update(dt, sensors);
        m_state_mutex.lock();
        m_state = state;
        m_state_mutex.unlock();

        sleep_periodic(m_period);
    }
}

bool
state_estimator_task::set_telemetry(payload_u& payload) const noexcept
{
    state_s state = get_state();
    PB_SET(payload.state, position, state.position);
    PB_SET(payload.state, velocity, state.velocity);
    PB_SET(payload.state, acceleration, state.acceleration);
    PB_SET(payload.state, angular_velocity, state.angular_velocity);
    PB_SET(payload.state, rotation, state.rotationq.as_vector());
    
    return true;
}

}