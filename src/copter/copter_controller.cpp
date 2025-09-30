#include "copter_controller.hpp"

namespace mp {

copter_controller::copter_controller() noexcept
{
    set_angular_controls({0, 0});
}

bool
copter_controller::set_angular_controls(angular_controls_s input) noexcept
{
    // TODO: Add bounds checking and return false if out of bounds
    emblib::rtos::scoped_lock lock(m_mutex);
    m_controls = input;
    return true;
}

bool
copter_controller::set_linear_controls(linear_controls_s input) noexcept
{
    // TODO: Add bounds checking and return false if out of bounds
    emblib::rtos::scoped_lock lock(m_mutex);
    m_controls = input;
    return true;
}

}