#pragma once

#include "common/config.hpp"
#include "link/telemetry.hpp"
#include "sensors/sensor_manager.hpp"
#include "state/state_estimator.hpp"
#include <emblib/rtos/mutex.hpp>
#include <emblib/rtos/task.hpp>

namespace mp {

/**
 * Periodically runs the state estimation algorithm and
 * safely exposes the calculated state using a mutex
 */
class state_estimator_task :
    private emblib::rtos::task,
    private telemetry_producer<mp_pb_telemetry_Channel_state_tag> {
public:
    template <size_t STACK_SIZE>
    explicit state_estimator_task(
        state_estimator& estimator,
        milliseconds_t period,
        const sensor_manager& sensor_manager,
        emblib::rtos::task_stack<STACK_SIZE>& stack,
        task_priority_e priority = TASK_PRIORITY_VERY_HIGH
    ) noexcept :
        task("state estimator task", priority, stack),
        m_estimator(estimator),
        m_sensor_manager(sensor_manager),
        m_period(period)
    {}

    /**
     * Get the current state
     */
    state_s get_state() const noexcept
    {
        emblib::rtos::scoped_lock lock(m_state_mutex);
        return m_state;
    }

private:
    void run() noexcept override;

    bool set_telemetry(payload_u& payload) const noexcept override;

private:
    // State estimation algorithm
    state_estimator& m_estimator;
    // Sensor manager
    const sensor_manager& m_sensor_manager;
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