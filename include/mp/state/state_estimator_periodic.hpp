#pragma once

#include "common/config.hpp"
#include "state/state_estimator.hpp"
#include <emblib/rtos/mutex.hpp>
#include <emblib/rtos/task.hpp>

namespace mp {

/**
 * Periodically runs the state estimation algorithm and
 * safely exposes the calculated state using a mutex
 */
class state_estimator_periodic : public state_estimator, private emblib::rtos::task {

public:
    template <size_t STACK_SIZE>
    explicit state_estimator_periodic(
        milliseconds_t period,
        emblib::rtos::task_stack<STACK_SIZE>& stack,
        task_priority_e task_priority = TASK_PRIORITY_VERY_HIGH
    ) noexcept :
        task("Periodic state estimator", task_priority, stack),
        m_period(period)
    {}

    /**
     * Get the current state
     */
    state_s get_state() const noexcept override
    {
        emblib::rtos::scoped_lock lock(m_state_mutex);
        return m_state;
    }

private:
    /**
     * Single iteration of the state estimation algorithm
     */
    virtual void iteration(float dt) noexcept = 0;

    /**
     * Create state from an internal representation
     */
    virtual state_s create_state() const noexcept = 0;

    /**
     * Task thread
     */
    void run() noexcept override;

private:
    // State is copied here after the algorithm iteration
    // to allow locking the mutex only during this copy
    // instead of during algorithm execution
    state_s m_state;
    // Mutex for reading and writing to the state copy
    mutable emblib::rtos::mutex m_state_mutex;
    // Execution period
    milliseconds_t m_period;
};

}