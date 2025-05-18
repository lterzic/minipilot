#pragma once

#include <emblib/utils/chrono.hpp>

namespace mp {

/**
 * Get the number of milliseconds since the application started
 * @note To be provided by the port
 * @todo To be moved to a singleton interface
 */
emblib::milliseconds_t get_time_since_start() noexcept;

}