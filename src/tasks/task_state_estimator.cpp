#include "task_state_estimator.hpp"
#include "logging/log.hpp"
#include "util/constants.hpp"
#include <cmath>

namespace mp {

void task_state_estimator::run() noexcept
{
    constexpr float dt = emblib::units::seconds<float>(TASK_STATE_PERIOD).value();
    
    while (true) {
        m_state_estimator.update(m_sensor_manager, dt);

        // Assign the estimator state to the readable state struct
        m_state_mutex.lock();
        m_state = m_state_estimator.get_state();
        m_state_mutex.unlock();

        sleep_periodic(TASK_STATE_PERIOD);
    }
}

}