#include <mp/common/chrono.hpp>
#include <FreeRTOSConfig.h>

namespace mp {

static const auto g_start = std::chrono::steady_clock::now();

/**
 * Using std clock for timekeeping
 */
milliseconds_t get_time_since_start() noexcept
{
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = now - g_start;
    return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
}

}

/**
 * In case that sysmon is not being used, task switch hook
 * is weak defined as an empty function.
 * @todo Could be totally removed
 */
__attribute__((weak)) void mp_on_task_switch()
{
    
}