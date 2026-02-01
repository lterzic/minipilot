#include "controls/copter_controller.hpp"
#include "logging/logging.hpp"

namespace mp {

copter_controller::copter_controller(copter& copter) :
    m_copter(copter)
{}

void
copter_controller::update(const controls_s& controls, const state_s& state, float dt)
{
    const auto actuation = update_copter(controls, state, dt);
    
    if (!m_copter.actuate(actuation)) {
        log_warning("Failed to apply actuation to the copter");
    }
}

}