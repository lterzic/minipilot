#pragma once

#include "pb/logging/log.nanopb.h"

namespace mp {

/**
 * Log level enum class wrapper
 */
enum class log_level_e {
    DEBUG   = mp_pb_logging_LogLevel_LOG_LEVEL_DEBUG,
    INFO    = mp_pb_logging_LogLevel_LOG_LEVEL_INFO,
    WARNING = mp_pb_logging_LogLevel_LOG_LEVEL_WARNING,
    ERROR   = mp_pb_logging_LogLevel_LOG_LEVEL_ERROR
};

/**
 * Log entry based on the protobuf definition
 */
using log_s = mp_pb_logging_Log;

}