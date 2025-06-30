#pragma once

#include "log_types.hpp"
#include <emblib/io/ostream.hpp>
#include <etl/list.h>

namespace mp {

/**
 * When log_handler formats a log into a character sequence
 * it can write it to multiple devices. Each device can require
 * a different level, for example we might send all >= INFO logs
 * to the GCS and only log >= WARNING into the blackbox, but use
 * the proto format for both.
 */
class log_device {

public:
    explicit log_device(emblib::io::ostream<char>& dev, log_level_e level) :
        m_dev(dev),
        m_level(level)
    {}

    void set_level(log_level_e level) noexcept
    {
        m_level = level;
    }

    void write(log_level_e level, const char* data, size_t length)
    {
        if (level < m_level) {
            return;
        }
        // TODO: Write async
        m_dev.write(data, length, emblib::io::timeout_t(10));
    }

private:
    // Minimum level for outputting the log
    log_level_e m_level;
    // Output device
    emblib::io::ostream<char>& m_dev;
};

/**
 * Converts the log from the internal format into a different
 * representation.
 * @todo Can be renamed to log_formatter
 */
class log_handler {

public:
    explicit log_handler(etl::ilist<log_device*>& devices) :
        m_devices(devices)
    {}

    /**
     * Process the log and send it to the appropriate output devices
     * @note Once the log has been converted to a serial format,
     * call `log_handler::flush`
     */
    virtual void handle_log(const log_s& log) noexcept = 0;

protected:
    /**
     * Sends the serial data to all the devices which accept this log level
     */
    void flush(log_level_e level, const char* data, size_t length) noexcept
    {
        for (auto device : m_devices) {
            device->write(level, data, length);
        }
    }

private:
    etl::ilist<log_device*>& m_devices;

};

}