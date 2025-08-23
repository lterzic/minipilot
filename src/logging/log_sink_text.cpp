#include "log_sink_text.hpp"
#include <etl/string.h>

namespace mp {

log_sink_text::log_sink_text(emblib::io::ostream& dev) :
    m_dev(dev)
{}

void log_sink_text::write(const log_s& log) noexcept
{
    // TODO: Stream characters instead of buffering into this string
    etl::string<sizeof(log.data) + 12> out_msg;
    
    // TODO: print time and ID
    static const char* level_prefix[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    out_msg = level_prefix[static_cast<int>(log.level)];
    out_msg += ": ";
    out_msg += log.data;
    out_msg += "\n";

    // Allowed timeout is (length in bytes divided by 8) milliseconds
    // which requires a minimum datarate of 1kbps
    auto timeout = milliseconds_t(out_msg.size() >> 3);
    m_dev.write(out_msg.c_str(), out_msg.size(), timeout);
}

}