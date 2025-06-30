#include "logger.hpp"
#include "util/chrono.hpp"

namespace mp {

logger& logger::get_instance() noexcept
{
    static logger s_logger;
    return s_logger;
}

logger::logger() noexcept :
    task("Task logger", 1, m_stack),
    m_message_count(0)
{
    m_format.precision(3);
}

void logger::flush_log(log_s& log, log_level_e level) noexcept
{
    log.level = level;
    log.time_since_start = get_time_since_start();
    
    // Message count is incremented here if running in bare-metal
    // mode since there are no other threads for a race condition,
    // else it is incremented in the task to avoid using a mutex here
    
    if (task::is_scheduler_running()) {
        m_queue.send(log, milliseconds_t(-1));
    } else {
        log.message_id = m_message_count++;
        
        for (log_handler* handler : m_handlers) {
            handler->handle_log(log);
        }
    }
}

void logger::run() noexcept
{
    log_s log;
    while (true) {
        // Wait indefinitely for a log
        m_queue.receive(log, milliseconds_t(-1));
        log.message_id = m_message_count++;

        for (log_handler* handler : m_handlers) {
            handler->handle_log(log);
        }
    }
}

}