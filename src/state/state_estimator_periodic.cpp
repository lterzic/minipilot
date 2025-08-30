#include "state/state_estimator_periodic.hpp"

namespace mp {

void state_estimator_periodic::run() noexcept
{
    float dt = emblib::units::seconds<float>(m_period).value();

    while (true) {
        iteration(dt);

        m_state_mutex.lock();
        m_state = create_state();
        m_state_mutex.unlock();

        sleep_periodic(m_period);
    }
}

}