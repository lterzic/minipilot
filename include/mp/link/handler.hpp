#pragma once

#include <pblink/link/link.nanopb.h>

namespace mp {

/**
 * Uplink message payload type
 */
using rx_payload_e = pblink::link::uplink_s::payload_e;

/**
 * Handler for one type of uplink message payload
 * @note Handlers are executed in the receiver thread context
 */
template <rx_payload_e PAYLOAD>
struct rx_handler {
    /**
     * Payload union
     */
    using payload_u = pblink::link::uplink_s::pblink_link_uplink_payload;

    /**
     * Process the received message
     * @note It is guaranteed that the payload matches the template
     * `PAYLOAD` type
     */
    virtual void handle(payload_u& payload) noexcept = 0;
};

}