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

        auto actuation = m_controller.update(controls, state, dt);
        m_copter.actuate(actuation);

        sleep_periodic(m_period);
    }
}

}