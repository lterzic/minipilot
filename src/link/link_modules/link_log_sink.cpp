#include "link_log_sink.hpp"
#include <pb_encode.h>

namespace mp {

link_log_sink::link_log_sink(tx& tx) :
    m_tx(tx)
{}

static bool encode_log_data(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
{
    if (!pb_encode_tag_for_field(stream, field))
        return false;

    const log_s* log = (log_s*)arg;
    return pb_encode_string(stream, (uint8_t*)log->data, strnlen(log->data, sizeof(log->data)));
}

void link_log_sink::write(const log_s& log) noexcept
{
    auto payload_cb = [&log](auto& payload, auto& payload_type) {
        payload_type = mp_pb_link_DownlinkMessage_log_tag;
        mp_pb_link_Log& msg = payload.log;

        msg.timestamp_ms = log.time_since_start.value();
        msg.level = static_cast<mp_pb_link_LogLevel>((int)log.level + 1);
        msg.message_id = log.message_id;
        msg.message.arg = const_cast<log_s*>(&log);
        msg.message.funcs.encode = &encode_log_data;
    };
    m_tx.send_downlink(payload_cb);
}

}