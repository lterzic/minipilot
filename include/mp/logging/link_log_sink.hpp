#pragma once

#include "link/handler.hpp"
#include "link/tx.hpp"
#include "log_sink.hpp"

namespace mp {

/**
 * Logging interface towards the link connection
 * @todo Move logging rx handling to the logger
 */
class link_log_sink :
    public rx_handler<rx_payload_e::LOGGING>,
    public log_sink {
public:
    link_log_sink(tx& tx) :
        m_tx(tx)
    {}

    /**
     * Send the log message over the link.
     */
    void write(const log_entry_s& log) noexcept override
    {
        m_tx.send([&log](tx::tx_message_s& msg) {
            msg.which_payload = pblink::link::downlink_s::payload_e::LOGGING;
            msg.payload.logging.which_payload = pblink::link::logging_downlink_s::payload_e::LOG;
            msg.payload.logging.payload.log = log;
        });
    }

    /**
     * Apply the received log level to this log sink.
     */
    void handle(payload_u& payload) noexcept override
    {
        pblink::link::logging_uplink_s& msg = payload.logging;
        switch (msg.which_payload) {
        case pblink::link::logging_uplink_s::payload_e::SET_LEVEL:
            set_level(log_level_e(msg.payload.set_level.level));
            break;
        default:
            break;
        }
    }

private:
    tx& m_tx;
};

}