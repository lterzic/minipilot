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

}