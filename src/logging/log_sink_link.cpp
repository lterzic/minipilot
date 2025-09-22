#include "logging/log_sink_link.hpp"
#include <pb_encode.h>

namespace mp {

log_sink_link::log_sink_link(tx& tx) :
    m_tx(tx)
{}

static bool encode_log_data(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
{
    if (!pb_encode_tag_for_field(stream, field))
        return false;

    const log_s* log = (log_s*)arg;
    return pb_encode_string(stream, (uint8_t*)log->data, strnlen(log->data, sizeof(log->data)));
}

void log_sink_link::write(const log_s& log) noexcept
{
    auto payload_cb = [&log](mp_pb_logging_Downlink& downlink) {
        mp_pb_logging_Log& msg = downlink.payload.log;
        downlink.which_payload = mp_pb_logging_Downlink_log_tag;

        msg.timestamp_ms = log.time_since_start.value();
        msg.level = static_cast<mp_pb_logging_LogLevel>((int)log.level + 1);
        msg.message_id = log.message_id;
        msg.message.arg = const_cast<log_s*>(&log);
        msg.message.funcs.encode = &encode_log_data;
    };
    m_tx.send_downlink<mp_pb_logging_Downlink>(payload_cb);
}

}