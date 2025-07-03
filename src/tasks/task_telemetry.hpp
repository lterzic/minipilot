#pragma once

#include "task_config.hpp"
#include "task_state_estimator.hpp"
#include "sensors/sensor_manager.hpp"
#include <emblib/io/ostream.hpp>
#include <emblib/rtos/task.hpp>

namespace mp {

// Change to task_transmitter and add queue for output messages
// Also pack each message into protobuf which describes the type
// Example: LogMessage, TelemetryMessage, ...
class task_telemetry : public emblib::task {

public:
    explicit task_telemetry(
        emblib::io::ostream<char>& telemetry_device,
        const sensor_manager& sensor_manager,
        task_state_estimator& task_state_estimator
    );

private:
    /**
     * Task implementation
     */
    void run() noexcept override;

private:
    emblib::task_stack_t<TASK_TELEMETRY_STACK_SIZE> m_task_stack;
    emblib::io::ostream<char>& m_telemetry_device;

    const sensor_manager& m_sensor_manager;
    task_state_estimator& m_task_state;

    size_t m_msg_id;
};

}