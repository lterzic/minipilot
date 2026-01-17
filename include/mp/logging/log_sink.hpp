#pragma once

#include "log_entry.hpp"

namespace mp {

/**
 * Takes in a log from the logger, converts it to the format
 * expected by the output and writes it
 */
class log_sink {

public:
    explicit log_sink() = default;
    virtual ~log_sink() = default;

    /**
     * Set the log level for this sink
     * @note This is used after the log level is checked by the
     * logger meaning the level of the log has to be greater or
     * equal to both the logger's and sink's levels to be written
     */
    void set_level(log_level_e level) noexcept
    {
        m_level = level;
    }

    /**
     * Get the log level for this sink
     */
    log_level_e get_level() const noexcept
    {
        return m_level;
    }

    /**
     * Write the log to this sink
     */
    virtual void write(const log_entry_s& log) noexcept = 0;

private:
    // Minimum level for outputting the log
    log_level_e m_level;
};

}