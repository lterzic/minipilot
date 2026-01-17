#include "state/estimator_runner.hpp"
#include "common/pb.hpp"
#include <etl/optional.h>

namespace mp {

bool
estimator_runner::produce(payload_u& payload) noexcept
{
    emblib::rtos::scoped_lock lock(m_lock);

    PB_SET(payload.state.kinematics, position, m_state.kinematics.position);
    PB_SET(payload.state.kinematics, velocity, m_state.kinematics.velocity);
    PB_SET(payload.state.kinematics, acceleration, m_state.kinematics.acceleration);
    PB_SET(payload.state.kinematics, rotation, m_state.kinematics.rotation.as_vector());
    PB_SET(payload.state.kinematics, angular_velocity, m_state.kinematics.angular_velocity);
    payload.state.has_kinematics = true;

    PB_SET(payload.state, gyroscope_drift, m_state.gyroscope_drift);
    payload.state.grounded = m_state.grounded;

    return true;
}

void
estimator_runner::run() noexcept
{
    float dt = emblib::units::seconds<float>(m_period).value();

    etl::optional<accelerometer_processor::reader> accelerometer_reader;
    float accelerometer_noise_density = 0.f;
    if (const auto& processor = m_sensor_manager.get_sensor<sensor_type_e::ACCELEROMETER>()) {
        accelerometer_reader.emplace(processor->get_reader());
        accelerometer_noise_density = processor->get_noise_density();
    }

    etl::optional<gyroscope_processor::reader> gyroscope_reader;
    float gyroscope_noise_density = 0.f;
    if (const auto& processor = m_sensor_manager.get_sensor<sensor_type_e::GYROSCOPE>()) {
        gyroscope_reader.emplace(processor->get_reader());
        gyroscope_noise_density = processor->get_noise_density();
    }

    while (true) {
        estimator::sensor_data_s sensor_data;
        
        if (accelerometer_reader) {
            if (auto data = accelerometer_reader->read_if_new()) {
                sensor_data.accelerometer = {data->cast<float>(), accelerometer_noise_density};
            }
        }

        if (gyroscope_reader) {
            if (auto data = gyroscope_reader->read_if_new()) {
                sensor_data.gyroscope = {data->cast<float>(), gyroscope_noise_density};
            }
        }

        auto state = m_estimator.update(dt, sensor_data);
        m_lock.lock();
        m_state = state;
        m_lock.unlock();

        sleep_periodic(m_period);
    }
}

}