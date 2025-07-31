#pragma once

#include "mp_config.hpp"
#include <emblib/units/time.hpp>

namespace mp {

enum class log_level_e {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

struct log_s {
    // Unique ID within this session
    size_t message_id;
    // Log level
    log_level_e level;
    // Log time
    emblib::units::milliseconds<uint32_t> time_since_start;
    // Log data
    char data[LOG_MAX_MESSAGE_LENGTH];

    // TODO: Add task which called log
};

}