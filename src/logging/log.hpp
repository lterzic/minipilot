#pragma once

#include "pb/logging/log.nanopb.h"

namespace mp {

/**
 * Log level enum class wrapper
 */
enum class log_level_e {
    DEBUG   = MP_PB_LOGGING_LOG_LEVEL_LOG_LEVEL_DEBUG,
    INFO    = MP_PB_LOGGING_LOG_LEVEL_LOG_LEVEL_INFO,
    WARNING = MP_PB_LOGGING_LOG_LEVEL_LOG_LEVEL_WARNING,
    ERROR   = MP_PB_LOGGING_LOG_LEVEL_LOG_LEVEL_ERROR
};

/**
 * Log entry based on the protobuf definition
 */
using log_s = pb::logging::log_s;

/**
 * Encode each type of log argument
 * @note Determined at compile time
 */
bool log_write_arg(log_s& log, int32_t value) noexcept;
bool log_write_arg(log_s& log, uint32_t value) noexcept;
bool log_write_arg(log_s& log, float value) noexcept;
bool log_write_arg(log_s& log, const char* value) noexcept;

/**
 * Encode a variable length string field
 * @todo Move to pb utils library in protobuf
 */
bool encode_string(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg);

}