#include "link/modules/link_logging.hpp"
#include <pb_encode.h>

namespace mp {

link_logging::link_logging(link& link, logger& logger) :
    m_tx(link.m_tx)
{
    logger.add_sink(*this);
    assert(link.m_rx.set_handler(mp_pb_link_Uplink_logging_tag, *this));
}

void
link_logging::write(const log_s& log) noexcept
{
    mp_pb_link_Downlink msg = {0};
    msg.which_payload = mp_pb_link_Downlink_logging_tag;
    
    mp_pb_link_LoggingDownlink& log_downlink = msg.payload.logging;
    log_downlink.which_payload = mp_pb_link_LoggingDownlink_log_tag;
    log_downlink.payload.log = log;
    
    m_tx.send_downlink(msg);
}

void
link_logging::handle(payload_u& payload) noexcept
{
    mp_pb_link_LoggingUplink& msg = payload.logging;

    switch (msg.which_payload) {
    case mp_pb_link_LoggingUplink_set_level_tag:
        set_level(log_level_e(msg.payload.set_level.level));
        break;
    default:
        break;
    }
}

}