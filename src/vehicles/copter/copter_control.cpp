#include "copter_control.hpp"

namespace mp {

void copter_control::run() noexcept
{
    float dt = emblib::seconds<float>(m_period).value();

    while (true) {
        state_s state = m_state_estimator.get_state();
        actuation_s actuation = iterate(state, dt);

        m_copter.actuate(actuation.thrust, actuation.torque);
        
        sleep_periodic(m_period);
    }
}

}