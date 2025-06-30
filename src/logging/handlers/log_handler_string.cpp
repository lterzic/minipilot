#include "log_handler_string.hpp"
#include <etl/string_stream.h>

namespace mp {

void log_handler_string::handle_log(const log_s& log) noexcept
{
    // TODO: Stream characters instead of buffering into this string
    etl::string<log.data.MAX_SIZE + 12> out_msg;
    
    // TODO: print time and ID
    static const char* level_prefix[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    out_msg = level_prefix[static_cast<int>(log.level)];
    out_msg += ": ";
    out_msg.append(log.data);
    out_msg += "\n";

    flush(log.level, out_msg.c_str(), out_msg.size());
}

}