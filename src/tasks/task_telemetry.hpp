#pragma once

#include "task_config.hpp"
#include "task_accelerometer.hpp"
#include "task_gyroscope.hpp"
#include "task_state_estimator.hpp"
#include "emblib/driver/char_dev.hpp"
#include "emblib/rtos/task.hpp"
#include "emblib/rtos/queue.hpp"
#include "pb/telemetry.pb.h"
#include "pb_encode.h"

namespace mp {

// Change to task_transmitter and add queue for output messages
// Also pack each message into protobuf which describes the type
// Example: LogMessage, TelemetryMessage, ...
class task_telemetry : public emblib::task {

public:
    explicit task_telemetry(
        emblib::char_dev& telemetry_device,
        task_accelerometer& task_accelerometer,
        task_gyroscope& task_gyroscope,
        task_state_estimator& task_state_estimator
    );

private:
    /**
     * Task implementation
     */
    void run() noexcept override;

private:
    emblib::task_stack_t<TASK_TELEMETRY_STACK_SIZE> m_task_stack;
    emblib::char_dev& m_telemetry_device;

    task_accelerometer& m_task_accel;
    task_gyroscope& m_task_gyro;
    task_state_estimator& m_task_state;
};

}