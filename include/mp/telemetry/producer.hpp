#pragma once

#include <pblink/telemetry/channel.nanopb.h>

namespace mp {

/**
 * Telemetry channel enumeration
 */
using telemetry_channel_e = pblink::telemetry::channel_s::payload_e;

/**
 * Producer for one telemetry channel
 */
template <telemetry_channel_e CHANNEL>
class telemetry_producer {
public:
    /**
     * Payload union
     */
    using payload_u = pblink::telemetry::channel_s::pblink_telemetry_channel_payload;

    /**
     * Callback to fill broadcast information about this
     * channel type. Broadcast information is set once on
     * telemetry init, and later only on request.
     * @todo Replace with separate payload_u type for broadcast messages
     */
    virtual bool broadcast(payload_u& broadcast_payload) const noexcept
    {
        // Broadcast information is optional
        return false;
    }

    /**
     * Callback which fills the channel with the data
     * according to the payload type
     * @returns `true` if payload valid and should be sent
     */
    virtual bool produce(payload_u& payload) noexcept = 0;
};

}