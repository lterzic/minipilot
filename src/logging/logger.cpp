#include "logger.hpp"
#include "common/chrono.hpp"
#include <emblib/rtos/scheduler.hpp>

namespace mp {

logger& logger::get_instance() noexcept
{
    static logger s_logger;
    return s_logger;
}

logger::logger() noexcept :
    static_task("Task logger", LOGGER_TASK_PRIORITY),
    m_message_count(0)
{
    m_format.precision(3);
}

static void
flush(const log_s& log, const etl::ilist<log_sink*>& sinks)
{
    log_level_e log_level = static_cast<log_level_e>(log.level);
    
    for (log_sink* sink : sinks) {
        if (sink->get_level() <= log_level) {
            sink->write(log);
        }
    }
}

void logger::flush_log(log_s& log, log_level_e level) noexcept
{
    log.level = decltype(log_s::level)(level);
    log.timestamp_ms = get_time_since_start().value();
    
    if (emblib::rtos::is_scheduler_running()) {
        m_queue.send(log, milliseconds_t(-1));
    } else {
        // Message count is incremented here if running in bare-metal
        // mode since there are no other threads for a race condition,
        // else it is incremented in the task to avoid using a mutex here
        log.message_id = m_message_count++;
        flush(log, m_sinks);
    }
}

void logger::run() noexcept
{
    log_s log;
    while (true) {
        // Wait indefinitely for a log
        m_queue.receive(log, milliseconds_t(-1));
        log.message_id = m_message_count++;
        flush(log, m_sinks);
    }
}

}