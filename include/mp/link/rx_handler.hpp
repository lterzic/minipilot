#pragma once

#include "pb/link/link.nanopb.h"

namespace mp {

/**
 * Uplink message payload type
 */
using rx_payload_e = pb::link::uplink_s::payload_e;

/**
 * Handler for one type of uplink message payload
 * @note Handlers are executed in the receiver thread context
 */
template <rx_payload_e PAYLOAD>
struct rx_handler {
    /**
     * Payload union
     */
    using payload_u = pb::link::uplink_s::mp_pb_link_uplink_payload;

    /**
     * Process the received messages
     * @note It is guaranteed that the payload matches the
     * register payload tag when calling `rx::set_handler`
     */
    virtual void handle(payload_u& payload) noexcept = 0;
};

}