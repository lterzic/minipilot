#pragma once

#include <emblib/units/time.hpp>
#include <etl/string.h>

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
    etl::string<100> data;

    // TODO: Add task which called log
};

}