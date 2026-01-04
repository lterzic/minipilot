#include "link/modules/link_logging.hpp"
#include <pb_encode.h>

namespace mp {

link_logging::link_logging(link& link, logger& logger) :
    m_tx(link.m_tx)
{
    logger.add_sink(*this);
    assert(link.m_rx.set_handler(*this));
}

void
link_logging::write(const log_s& log) noexcept
{
    m_tx.send([&log](tx::tx_message_s& msg) {
        msg.which_payload = pb::link::downlink_s::payload_e::LOGGING;
        msg.payload.logging.which_payload = pb::link::logging_downlink_s::payload_e::LOG;
        msg.payload.logging.payload.log = log;
    });
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