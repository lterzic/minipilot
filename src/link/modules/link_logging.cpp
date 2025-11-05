#include "link/modules/link_logging.hpp"
#include <pb_encode.h>

namespace mp {

link_logging::link_logging(link& link, logger& logger) :
    m_tx(link.m_tx)
{
    logger.add_sink(*this);
    assert(link.m_rx.set_handler(mp_pb_link_Uplink_logging_tag, *this));
}

static bool
encode_log_data(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
{
    if (!pb_encode_tag_for_field(stream, field))
        return false;

    const log_s* log = (log_s*)arg;
    return pb_encode_string(stream, (uint8_t*)log->data, strnlen(log->data, sizeof(log->data)));
}

void
link_logging::write(const log_s& log) noexcept
{
    mp_pb_link_Downlink msg = {0};
    msg.which_payload = mp_pb_link_Downlink_logging_tag;
    
    mp_pb_logging_Downlink& log_downlink = msg.payload.logging;
    log_downlink.which_payload = mp_pb_logging_Downlink_log_tag;
    
    mp_pb_logging_Log& log_msg = log_downlink.payload.log;
    log_msg.timestamp_ms = log.time_since_start.value();
    log_msg.level = static_cast<mp_pb_logging_LogLevel>((int)log.level + 1);
    log_msg.message_id = log.message_id;
    log_msg.message.arg = const_cast<log_s*>(&log);
    log_msg.message.funcs.encode = &encode_log_data;
    
    m_tx.send_downlink(msg);
}

void
link_logging::handle(payload_u& payload) noexcept
{
    mp_pb_logging_Uplink& msg = payload.logging;

    switch (msg.which_payload) {
    case mp_pb_logging_Uplink_set_level_tag:
        if (msg.payload.set_level.level > mp_pb_logging_LogLevel_LOG_LEVEL_UNKNOWN) {
            log_level_e level = log_level_e(msg.payload.set_level.level - 1);
            set_level(level);
        }
        break;
    default:
        break;
    }
}

}