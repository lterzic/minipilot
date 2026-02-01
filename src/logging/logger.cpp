#include "logging/logger.hpp"
#include "common/chrono.hpp"
#include "common/pb.hpp"
#include <emblib/rtos/scheduler.hpp>

namespace mp {

logger& logger::get_instance() noexcept
{
    static logger s_logger;
    return s_logger;
}

logger::logger() noexcept :
    static_task("logger", LOGGER_TASK_PRIORITY),
    m_message_count(0)
{}

void logger::add_sink(log_sink& sink) noexcept
{
    m_sinks.push_back(&sink);
}

static void write_to_sinks(const log_entry_s& log, const etl::ilist<log_sink*>& sinks)
{
    log_level_e log_level = static_cast<log_level_e>(log.level);

    for (log_sink* sink : sinks) {
        if (sink->get_level() <= log_level) {
            sink->write(log);
        }
    }
}

void logger::flush(log_level_e level, log_entry_s* log) noexcept
{
    log->level = decltype(log_entry_s::level)(level);
    log->has_timestamp = true;
    log->timestamp = pb_from(get_time_since_start());
    log->format.funcs.encode = &pb_encode_string;
    
    if (emblib::rtos::is_scheduler_running()) {
        m_queue.send(log, milliseconds_t(-1));
    } else {
        // Message count is incremented here if running in bare-metal
        // mode since there are no other threads for a race condition,
        // else it is incremented in the task to avoid using a mutex here
        log->message_id = m_message_count++;
        write_to_sinks(*log, m_sinks);
        m_alloc.dealloc(log);
    }
}

void logger::run() noexcept
{
    while (true) {
        log_entry_s* log;
        m_queue.receive(log, milliseconds_t(-1));

        log->message_id = m_message_count++;
        write_to_sinks(*log, m_sinks);
        m_alloc.dealloc(log);
    }
}

}