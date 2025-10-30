#include "copter/copter_controller_task.hpp"

namespace mp {

void
copter_controller_task::run() noexcept
{
    // Assuming delta time doesn't change between iterations
    float dt = emblib::units::seconds<float>(m_period).value();

    while (true) {
        state_s state = m_state_estimator_task.get_state();

        m_control_mutex.lock();
        auto controls = m_controls;
        m_control_mutex.unlock();

        if (controls.is_type<copter_controller::angular_controls_s>()) {
            auto actuation = m_controller.update_angular(etl::get<0>(controls), state, dt);
            m_copter.actuate(actuation);
        } else {
            auto actuation = m_controller.update_linear(etl::get<1>(controls), state, dt);
            m_copter.actuate(actuation);
        }

        sleep_periodic(m_period);
    }
}

}