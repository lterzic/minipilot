#pragma once

#include "common/config.hpp"
#include "copter/copter.hpp"
#include "copter/copter_controller.hpp"
#include <emblib/rtos/task.hpp>

namespace mp {

/**
 * Periodically executes the control algorithm and sends
 * the actuation data to the vehicle.
 * @note If the vehicle type is known at compile time, it
 * should be passed as the template argument
 */
template <
    size_t STACK_SIZE,
    typename controller_type = copter_controller,
    typename copter_type = copter
>
class copter_controller_task : private emblib::rtos::static_task<STACK_SIZE> {
public:
    copter_controller_task(
        controller_type& controller,
        copter_type& copter,
        milliseconds_t period,
        task_priority_e priority = TASK_PRIORITY_HIGH
    ) noexcept :
        copter_controller_task::static_task("copter controller task", priority),
        m_controller(controller),
        m_copter(copter),
        m_period(period)
    {}

private:
    void run() noexcept override
    {
        // Assuming delta time doesn't change between iterations
        float dt = emblib::units::seconds<float>(m_period).value();

        while (true) {
            auto actuation = m_controller.iterate(dt);
            m_copter.actuate(actuation.thrust, actuation.torque);

            this->sleep_periodic(m_period);
        }
    }

private:
    controller_type& m_controller;
    copter_type& m_copter;
    milliseconds_t m_period;
};

}