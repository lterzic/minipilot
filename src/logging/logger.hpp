#pragma once

#include "log_handler.hpp"
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/queue.hpp>
#include <etl/list.h>
#include <etl/string_stream.h>

namespace mp {

class logger : public emblib::task {

public:
    /**
     * Get the singleton logger instance
     */
    static logger& get_instance() noexcept;

    /**
     * Log a message consisting of multiple items
     */
    template <typename... item_types>
    void log(log_level_e level, item_types&&... items) noexcept
    {
        if (m_handlers.empty()) {
            return;
        }

        log_s log;
        etl::string_stream log_stream(log.data, m_format);

        // Fill the log string with each item
        ((log_stream << items), ...);
        
        flush_log(log, level);
    }

    /**
     * Subscribe a new handler to the log event
     */
    void add_log_handler(log_handler& handler) noexcept
    {
        m_handlers.push_back(&handler);
    }

private:
    logger() noexcept;

    /**
     * Format and write log to output devices
     */
    void flush_log(log_s& log, log_level_e level) noexcept;

    /**
     * Task method
     */
    void run() noexcept override;

private:
    // Log message count
    size_t m_message_count;

    // Format for writing numeric types
    etl::format_spec m_format;
    
    // List of all subscribed handlers
    // TODO: Can be replaced by etl::ilist<>* and instead of having
    // `add_log_handler`, use `set_log_handlers(etl::ilist<>*)`
    etl::list<log_handler*, 4> m_handlers;

    // Log queue
    emblib::queue<log_s, 4> m_queue;

    // Task stack
    emblib::task_stack_t<1024> m_stack;
};

}