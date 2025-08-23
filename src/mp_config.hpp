#pragma once

#include "util/chrono.hpp"
#include <cstddef>

namespace mp {

/**
 * Priority naming
 */
enum task_priority_e : size_t {
    TASK_PRIORITY_VERY_LOW  = 1,
    TASK_PRIORITY_LOW       = 2,
    TASK_PRIORITY_MEDIUM    = 3,
    TASK_PRIORITY_HIGH      = 4,
    TASK_PRIORITY_VERY_HIGH = 5
};

inline constexpr task_priority_e    HANDSHAKE_PRIORITY      = TASK_PRIORITY_VERY_HIGH;
inline constexpr task_priority_e    RX_TASK_PRIORITY        = TASK_PRIORITY_HIGH;
inline constexpr milliseconds_t     RX_FAIL_SLEEP           = milliseconds_t(50);
inline constexpr milliseconds_t     TX_WRITE_TIMEOUT        = milliseconds_t(50);

inline constexpr task_priority_e    TELEMETRY_TASK_PRIORITY = TASK_PRIORITY_MEDIUM;
// TODO: Move to dynamic configuration
inline constexpr milliseconds_t     TELEMETRY_PERIOD        = milliseconds_t(200);
// TODO: Add TELEMETRY_SEND_ACCELEROMETER, TELEMETRY_SEND_GYROSCOPE, ...

inline constexpr size_t             LOG_MAX_MESSAGE_LENGTH  = 100;
inline constexpr task_priority_e    LOGGER_TASK_PRIORITY    = TASK_PRIORITY_LOW;
inline constexpr size_t             LOGGER_QUEUE_SIZE       = 4;
inline constexpr size_t             LOGGER_MAX_SINKS        = 4;

inline constexpr task_priority_e    ACCELEROMETER_READER_PRIORITY   = TASK_PRIORITY_VERY_HIGH;
inline constexpr milliseconds_t     ACCELEROMETER_READER_PERIOD     = milliseconds_t(10);
inline constexpr task_priority_e    GYROSCOPE_READER_PRIORITY       = TASK_PRIORITY_VERY_HIGH;
inline constexpr milliseconds_t     GYROSCOPE_READER_PERIOD         = milliseconds_t(10);

// TODO: Add state estimator priorities here

inline constexpr task_priority_e    COPTER_CONTROL_PRIORITY         = TASK_PRIORITY_HIGH;
inline constexpr milliseconds_t     COPTER_CONTROL_PID_PERIOD       = milliseconds_t(40);

}