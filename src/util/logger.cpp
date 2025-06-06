#include "logger.hpp"
#include "util/chrono.hpp"
#include "pb/mp/log.nanopb.h"
#include <pb_encode.h>

namespace mp {

logger& logger::get_instance() noexcept
{
    static logger s_logger;
    return s_logger;
}

// Move to pb_util
static bool encode_string(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) {
    auto buffer = static_cast<const logger::buffer_t*>(*arg);
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, (const uint8_t *)buffer->c_str(), buffer->length());
}

void logger::flush(log_level_e level, const buffer_t& buffer, emblib::io_dev& log_device) noexcept
{
    // Don't wait if cannot write currently
    static constexpr auto WRITE_TIMEOUT = std::chrono::milliseconds(0);

    if (m_encode_protobuf) {
        pb_mp_Log pb_msg = pb_mp_Log_init_zero;

        pb_msg.level = static_cast<pb_mp_LogLevel>(level);
        pb_msg.message_id = m_message_id;
        pb_msg.timestamp_ms = get_time_since_start().value();

        pb_msg.message.funcs.encode = &encode_string;
        pb_msg.message.arg = (void*)&buffer;

        char out_buffer[sizeof(pb_mp_Log) + sizeof(buffer_t)];
        pb_ostream_t pb_ostream = pb_ostream_from_buffer((pb_byte_t*)out_buffer, sizeof(out_buffer));
        
        if (pb_encode(&pb_ostream, pb_mp_Log_fields, &pb_msg)) {
            log_device.write(out_buffer, pb_ostream.bytes_written, WRITE_TIMEOUT);
        }
    } else {
        // Using the same size for the formatted message as for the protobuf approximately
        static etl::string<LOGGER_MAX_TOTAL_SIZE> formatted_msg_buffer;
        
        // TODO: print time and ID
        static const char* level_prefix[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
        formatted_msg_buffer = level_prefix[static_cast<int>(level)];
        formatted_msg_buffer += ": ";
        formatted_msg_buffer += buffer;
        formatted_msg_buffer += "\n";
        
        log_device.write(formatted_msg_buffer.c_str(), formatted_msg_buffer.size(), WRITE_TIMEOUT);
        formatted_msg_buffer.clear();
    }

    m_message_id++;
}


}