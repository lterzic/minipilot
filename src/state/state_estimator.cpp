#include "state/state_estimator.hpp"

namespace mp {

void
state_estimator::run() noexcept
{
    float dt = emblib::units::seconds<float>(m_period).value();

    size_t acc_count = 0;
    size_t gyro_count = 0;
    while (true) {
        state_estimator_algo::sensors_s sensors;
        
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

        auto state = m_algorithm.update(dt, sensors);
        m_state_mutex.lock();
        m_state = state;
        m_state_mutex.unlock();

        sleep_periodic(m_period);
    }
}

}