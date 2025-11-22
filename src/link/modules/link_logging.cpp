#include "link/modules/link_logging.hpp"
#include <pb_encode.h>

namespace mp {

link_logging::link_logging(link& link, logger& logger) :
    m_tx(link.m_tx)
{
    logger.add_sink(*this);
    assert(link.m_rx.set_handler(pb::link::uplink_s::payload_e::LOGGING, *this));
}

void
link_logging::write(const log_s& log) noexcept
{
    pb::link::downlink_s msg = {0};
    msg.which_payload = pb::link::downlink_s::payload_e::LOGGING;
    
    pb::link::logging_downlink_s& log_downlink = msg.payload.logging;
    log_downlink.which_payload = pb::link::logging_downlink_s::payload_e::LOG;
    log_downlink.payload.log = log;
    
    m_tx.send_downlink(msg);
}

void
link_logging::handle(payload_u& payload) noexcept
{
    pb::link::logging_uplink_s& msg = payload.logging;

    switch (msg.which_payload) {
    case pb::link::logging_uplink_s::payload_e::SET_LEVEL:
        set_level(log_level_e(msg.payload.set_level.level));
        break;
    default:
        break;
    }
}

}