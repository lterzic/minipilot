#pragma once

#include <pblink/logging/log.nanopb.h>

namespace mp {

/**
 * Log level enum class wrapper
 */
enum class log_level_e {
    DEBUG   = PBLINK_LOGGING_LOG_LEVEL_LOG_LEVEL_DEBUG,
    INFO    = PBLINK_LOGGING_LOG_LEVEL_LOG_LEVEL_INFO,
    WARNING = PBLINK_LOGGING_LOG_LEVEL_LOG_LEVEL_WARNING,
    ERROR   = PBLINK_LOGGING_LOG_LEVEL_LOG_LEVEL_ERROR
};

/**
 * Log entry based on the protobuf definition
 */
using log_entry_s = pblink::logging::log_s;

/**
 * Encode each type of log argument
 */
bool log_write_arg(log_entry_s& log, int32_t value) noexcept;
bool log_write_arg(log_entry_s& log, uint32_t value) noexcept;
bool log_write_arg(log_entry_s& log, float value) noexcept;
bool log_write_arg(log_entry_s& log, const char* value) noexcept;

}