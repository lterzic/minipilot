#include <FreeRTOSConfig.h>
#include <mp/common/chrono.hpp>
#include <mp/telemetry/sysmon.hpp>

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

void mp_on_task_switch()
{
    mp::sysmon::get_instance().on_task_switch();
}