#pragma once

#include "log_sink.hpp"
#include "mp_config.hpp"
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/queue.hpp>
#include <etl/list.h>
#include <etl/string_stream.h>

namespace mp {

class logger : private emblib::rtos::static_task<1024> {

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
        if (m_sinks.empty()) {
            return;
        }

        log_s log;
        // ETL string size doesn't include null termination character
        etl::string_ext log_data(log.data, sizeof(log.data) - 1);
        etl::string_stream log_stream(log_data, m_format);

        // Fill the log string with each item
        ((log_stream << items), ...);
        
        flush_log(log, level);
    }

    /**
     * Subscribe a new sink to the log event
     */
    void add_sink(log_sink& sink) noexcept
    {
        m_sinks.push_back(&sink);
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
    etl::list<log_sink*, LOGGER_MAX_SINKS> m_sinks;

    // Log queue
    emblib::rtos::queue<log_s, LOGGER_QUEUE_SIZE> m_queue;
};

}