#include "copter_controller.hpp"

namespace mp {

void copter_controller::run() noexcept
{
    float dt = emblib::units::seconds<float>(m_period).value();

    while (true) {
        state_s state = m_state_estimator.get_state();
        actuation_s actuation = iterate(state, dt);

        m_copter.actuate(actuation.thrust, actuation.torque);
        
        sleep_periodic(m_period);
    }
}

}