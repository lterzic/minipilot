#pragma once

#include "task_config.hpp"
#include "sensors/sensor_manager.hpp"
#include "state/state_estimator.hpp"
#include <emblib/rtos/mutex.hpp>
#include <emblib/rtos/task.hpp>

namespace mp {

/**
 * Task responsible for getting the sensor data and estimating the model state
 */
class task_state_estimator : public emblib::task {

public:
    // TODO: Add an initial state parameter
    explicit task_state_estimator(
        const sensor_manager& sensor_manager,
        state_estimator& state_estimator
    ) noexcept :
        task("Task state estimator", TASK_STATE_PRIORITY, m_task_stack),
        m_sensor_manager(sensor_manager),
        m_state_estimator(state_estimator)
    {}

    /**
     * Get the current state
     * @todo Maybe return as reference
     */
    state_s get_state() const noexcept
    {
        emblib::scoped_lock lock(m_state_mutex);
        return m_state;
    }

private:
    /**
     * Task thread
     */
    void run() noexcept override;

private:
    emblib::task_stack_t<TASK_STATE_STACK_SIZE> m_task_stack;

    const sensor_manager& m_sensor_manager;
    state_s m_state;
    state_estimator& m_state_estimator;
    mutable emblib::mutex m_state_mutex;
};

}