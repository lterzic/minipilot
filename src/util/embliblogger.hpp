#pragma once

#include <emblib/io/ostream.hpp>
#include <emblib/rtos/mutex.hpp>
#include <etl/string.h>
#include <etl/to_string.h>

namespace emblib {

enum class log_level_e {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

template <size_t BUFFER_SIZE>
class logger {

public:
    /** @todo Move to protected */
    using buffer_t = etl::string<BUFFER_SIZE>;

    explicit logger(io::ostream<char>* log_device) :
        m_log_device(log_device)
    {
        m_format.precision(3);
    }

    template <typename... item_types>
    void log(log_level_e level, item_types&&... items) noexcept
    {
        if (!m_log_device || level < m_output_level) {
            return;
        }

        m_mutex.lock();

        (log_item(items), ...);
        flush(level, m_buffer, *m_log_device);
        m_buffer.clear();
        
        /** @todo Instead of clear can remove number of written
         * bytes from the beginning of the buffer */

        m_mutex.unlock();
    }

    void set_output_level(log_level_e level) noexcept
    {
        m_output_level = level;
    }

    void set_output_device(io::ostream<char>& device) noexcept
    {
        m_log_device = &device;
    }

    void set_numeric_precision(size_t decimal_digits) noexcept
    {
        m_format.precision(decimal_digits);
    }

private:
    void log_item(const char* msg) noexcept
    {
        m_buffer.append(msg);
    }

    template <size_t N>
    void log_item(const char (&msg)[N]) noexcept
    {
        m_buffer.append(msg);
    }

    template <typename num_type, typename = typename std::enable_if<std::is_arithmetic<num_type>::value, num_type>::type>
    void log_item(num_type number) noexcept
    {
        etl::to_string(number, m_buffer, m_format, true);
    }

    virtual void flush(log_level_e level, const buffer_t& buffer, io::ostream<char>& log_device) noexcept
    {
        log_device.write(buffer.c_str(), buffer.size(), io::timeout_t(0));
    }

private:
    log_level_e m_output_level = log_level_e::INFO;
    io::ostream<char>* m_log_device;
    buffer_t m_buffer;
    mutex m_mutex;
    etl::format_spec m_format;
};

}