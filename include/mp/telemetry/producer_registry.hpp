#pragma once

#include <etl/vector.h>
#include <etl/unordered_map.h>
#include <pblink/telemetry/channel.nanopb.h>

namespace mp {

/**
 * Telemetry channel enumeration.
 */
using telemetry_channel_e = pblink::telemetry::channel_s::payload_e;

class telemetry_producer_base;

template <telemetry_channel_e CHANNEL>
class telemetry_producer;

/**
 * Global registry of all producers available in the system.
 */
class telemetry_producer_registry {
    /**
     * Each channel type might have 0 or more producers, and the
     * user subscribes to a (channel, source idx) pair to select
     * the source of the telemetry data.
     */
    using producer_map = etl::unordered_map<
        telemetry_channel_e,
        etl::vector<telemetry_producer_base*, 2>,
        pblink::telemetry::channel_s::PAYLOAD_COUNT
    >;

public:
    /**
     * Get the singleton registry instance.
     */
    static telemetry_producer_registry& get_instance() noexcept;

    /**
     * Add a channel producer
     */
    template <telemetry_channel_e CHANNEL>
    bool add_producer(telemetry_producer<CHANNEL>& producer) noexcept
    {
        return add_producer(CHANNEL, producer);
    }

    /**
     * Get telemetry channel to producer vector map.
     */
    const auto& get_producers() const noexcept
    {
        return m_producers;
    }

private:
    /**
     * Producer template parameter is not used at runtime currently, so
     * public add_producer calls are forwarded to this
     */
    bool add_producer(telemetry_channel_e channel, telemetry_producer_base& producer) noexcept;

private:
    producer_map m_producers;
};

}