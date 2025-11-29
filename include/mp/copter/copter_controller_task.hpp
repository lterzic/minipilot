#pragma once

#include "common/config.hpp"
#include "copter/copter.hpp"
#include "copter/copter_controller.hpp"
#include "state/state_estimator_task.hpp"
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/mutex.hpp>

namespace mp {

/**
 * Periodically executes the control algorithm and sends
 * the actuation data to the vehicle
 */
class copter_controller_task : private emblib::rtos::task {
public:
    template <size_t STACK_SIZE>
    copter_controller_task(
        copter_controller& controller,
        copter& copter,
        const state_estimator_task& state_estimator_task,
        milliseconds_t period,
        emblib::rtos::task_stack<STACK_SIZE>& stack,
        task_priority_e priority = TASK_PRIORITY_HIGH
    ) noexcept :
        task("copter controller", priority, stack),
        m_controller(controller),
        m_copter(copter),
        m_state_estimator_task(state_estimator_task),
        m_period(period)
    {}

    /**
     * Set the desired angular velocity vector and thrust
     * @returns `false` if the given angular velocity or thrust can't be achieved
     */
    bool set_angular_controls(copter_controller::angular_controls_s input) noexcept
    {
        // TODO: Add bounds checking and return false if out of bounds
        emblib::rtos::scoped_lock lock(m_control_mutex);
        m_controls = input;
        return true;
    }

    /**
     * Set the desired velocity vector and direction
     * @returns `false` if the given velocity can't be achieved
     */
    bool set_linear_controls(copter_controller::linear_controls_s input) noexcept
    {
        // TODO: Add bounds checking and return false if out of bounds
        emblib::rtos::scoped_lock lock(m_control_mutex);
        m_controls = input;
        return true;
    }

private:
    void run() noexcept override;

private:
    copter_controller& m_controller;
    copter& m_copter;
    const state_estimator_task& m_state_estimator_task;
    mutable emblib::rtos::mutex m_control_mutex;
    copter_controller::controls_v m_controls;
    milliseconds_t m_period;
};

}