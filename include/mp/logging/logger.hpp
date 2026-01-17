#pragma once

#include "common/config.hpp"
#include "log_entry.hpp"
#include "log_sink.hpp"
#include <emblib/lockfree/allocator.hpp>
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/queue.hpp>
#include <etl/list.h>

namespace mp {

/**
 * Logger provides methods to create a log and send it
 * to all registered sinks
 */
class logger : private emblib::rtos::static_task<1024> {
public:
    /**
     * Get the singleton logger instance
     */
    static logger& get_instance() noexcept;

    /**
     * Create a log entry of a specified level using std::format style
     * arguments and flush it to all registered sinks whose level
     * accepts the given log level.
     * @note If the logger is called before the scheduler is started
     * logs are written immediatelly (in blocking mode), else the log
     * entry is placed into a queue which is consumed once the log task
     * gets running time.
     * @todo Add `size_t FMT_SIZE` as template parameter and take in
     * `const char (&fmt)[FMT_SIZE]` to avoid using strlen when writing
     */
    template <typename... arg_types>
    void log(log_level_e level, const char* fmt, arg_types&&... args) noexcept;

    /**
     * Subscribe a new sink to the log event
     */
    void add_sink(log_sink& sink) noexcept;

private:
    /**
     * Default constructor
     */
    logger() noexcept;

    /**
     * Task method
     */
    void run() noexcept override;

    /**
     * Add additional metadata to the log and output it to
     * the appropriate sinks
     */
    void flush(log_level_e level, log_entry_s* log) noexcept;

private:
    // Log message count
    size_t m_message_count;
    
    // List of all subscribed handlers
    etl::list<log_sink*, LOGGER_MAX_SINKS> m_sinks;

    // Zero-copy queue using lock-free alloc and pointer queues
    emblib::rtos::queue<log_entry_s*, LOGGER_QUEUE_SIZE> m_queue;
    emblib::lockfree::allocator<log_entry_s, LOGGER_QUEUE_SIZE> m_alloc;
};

/**
 * Try to allocate data for the log and fill only the arguments
 * to reduce this templated function. Other metadata is then filled
 * in `flush`.
 */
template <typename... arg_types>
inline void logger::log(log_level_e level, const char* fmt, arg_types &&...args) noexcept
{
    if (m_sinks.empty()) {
        return;
    }

    if (log_entry_s* log = m_alloc.alloc()) {
        log->format.arg = const_cast<char*>(fmt);
        log->args_count = 0;
        (log_write_arg(*log, args), ...);
        flush(level, log);
    }    
}

}