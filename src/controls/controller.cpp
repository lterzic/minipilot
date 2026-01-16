#include "controls/controller.hpp"

namespace mp {

void
controller::set_transition_callback(transition_callback callback)
{
    m_callback = std::move(callback);
}

void
controller::notify_transition(const controls_s& new_controls)
{
    m_callback(new_controls);
}

}