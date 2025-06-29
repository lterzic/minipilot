#pragma once

#include <emblib/units/time.hpp>

namespace mp {

/**
 * Unsigned integer milliseconds type
 */
using milliseconds_t = emblib::units::milliseconds<size_t>;

/**
 * Get the number of milliseconds since the application started
 * @note To be provided by the port
 * @todo To be moved to a singleton interface
 */
milliseconds_t get_time_since_start() noexcept;

}