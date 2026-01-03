#pragma once

#include "common/config.hpp"
#include "estimator.hpp"
#include "sensors/sensor_manager.hpp"
#include "telemetry/producer.hpp"
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/spinlock.hpp>

namespace mp {

/**
 * Periodically fetches all available sensor data and
 * runs the state estimation algorithm
 */
class estimator_runner :
    private emblib::rtos::task,
    public telemetry_producer<telemetry_channel_e::STATE> {
public:
    template <size_t STACK_SIZE>
    explicit estimator_runner(
        estimator& estimator,
        const sensor_manager& sensor_manager,
        milliseconds_t period,
        emblib::rtos::task_stack<STACK_SIZE>& stack,
        task_priority_e priority = TASK_PRIORITY_VERY_HIGH
    ) noexcept :
        task("estimator runner", priority, stack),
        m_estimator(estimator),
        m_sensor_manager(sensor_manager),
        m_period(period)
    {}

    /**
     * Get the current state
     */
    state_s get_state() const noexcept
    {
        emblib::rtos::scoped_lock lock(m_lock);
        return m_state;
    }

    /**
     * Fill the state data
     */
    bool produce(payload_u& payload) const noexcept override;

private:
    /**
     * Runner loop
     */
    void run() noexcept override;

private:
    // State estimation algorithm
    estimator& m_estimator;
    // Sensor manager
    const sensor_manager& m_sensor_manager;
    // State is copied here after the algorithm iteration
    // to allow locking the mutex only during this copy
    // instead of during algorithm execution
    state_s m_state;
    // Mutex for reading and writing to the state copy
    mutable emblib::rtos::spinlock m_lock;
    // Execution period
    milliseconds_t m_period;
};

}