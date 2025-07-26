#pragma once

#include "task_config.hpp"
#include "state/state_estimator.hpp"
#include <emblib/rtos/mutex.hpp>
#include <emblib/rtos/task.hpp>

namespace mp {

/**
 * Periodically runs the state estimation algorithm and
 * safely exposes the calculated state using a mutex
 */
template <size_t STACK_SIZE>
class state_estimator_periodic : public state_estimator, public emblib::task {

public:
    explicit state_estimator_periodic(
        milliseconds_t period,
        task_priority_e task_priority = task_priority_e::TASK_PRIORITY_REALTIME
    ) noexcept :
        task("Periodic state estimator", task_priority, m_stack),
        m_period(period)
    {}

    /**
     * Get the current state
     */
    state_s get_state() const noexcept override
    {
        emblib::scoped_lock lock(m_state_mutex);
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
    void run() noexcept override
    {
        float dt = emblib::seconds<float>(m_period).value();
    
        while (true) {
            iteration(dt);

            m_state_mutex.lock();
            m_state = create_state();
            m_state_mutex.unlock();

            sleep_periodic(m_period);
        }
    }

private:
    // State is copied here after the algorithm iteration
    // to allow locking the mutex only during this copy
    // instead of during algorithm execution
    state_s m_state;
    // Mutex for reading and writing to the state copy
    mutable emblib::mutex m_state_mutex;
    // Task stack
    emblib::task_stack_t<STACK_SIZE> m_stack;
    // Execution period
    milliseconds_t m_period;
};

}