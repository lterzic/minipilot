#pragma once

#include "producer_registry.hpp"

namespace mp {

/**
 * Telemetry producer interface.
 * @note Users should not derive from this class, but from
 * template specialized `telemetry_producer`.
 */
class telemetry_producer_base {
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
     * @todo Change to const
     */
    virtual bool produce(payload_u& payload) noexcept = 0;
};

/**
 * Producer of one telemetry channel.
 */
template <telemetry_channel_e CHANNEL>
class telemetry_producer : public telemetry_producer_base {
public:
    /**
     * Register this producer on creation.
     */
    telemetry_producer()
    {
        telemetry_producer_registry::get_instance().add_producer(*this);
    }
};

}