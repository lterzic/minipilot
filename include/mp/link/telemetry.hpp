#pragma once

#include "common/config.hpp"
#include "link/link.hpp"
#include <emblib/rtos/task.hpp>
#include <etl/set.h>
#include <etl/unordered_map.h>
#include <etl/vector.h>

namespace mp {

/**
 * Producer for one telemetry channel
 */
struct telemetry_producer {
    /**
     * Payload union
     */
    using payload_u = mp_pb_telemetry_Channel::_mp_pb_telemetry_Channel_payload;

    /**
     * Callback which fills the channel with the data
     * according to the payload type
     * @returns `true` if payload valid and should be sent
     */
    virtual bool produce(payload_u& payload) const noexcept = 0;
};

class telemetry : private emblib::rtos::static_task<1024> {
public:
    /**
     * Map size should be equal to (or at least greater than) the number of channel types
     * Currently only allowing one source per channel type
     */
    using channel_map = etl::unordered_map<pb_size_t, etl::vector<telemetry_producer*, 1>, 4>;

public:
    explicit telemetry(link& link);

    /**
     * Add a subscriber for a specific channel
     * @todo Add frequency as a parameter
     */
    bool add_subscriber(pb_size_t channel_tag, size_t producer_id) noexcept;

    /**
     * Add a channel source (producer)
     */
    bool add_producer(pb_size_t channel_tag, telemetry_producer& producer) noexcept;

private:
    /**
     * Send the list of channels over the link
     */
    void broadcast() const noexcept;

    /**
     * Periodically emit all channels which have a subscription
     */
    void run() noexcept override;

    /**
     * Encoder for telemetry data. Part of the class to provide access
     * to channels and subscription members
     */
    static bool encode_channels(pb_ostream_t* stream, const pb_field_t*, void* const* arg) noexcept;

private:
    // Reference to the link transmitter
    tx& m_tx;
    // Map channel types to a vector of channel sources
    channel_map m_channels;
    // List of (channel tag, channel source id) subscriptions
    etl::set<etl::pair<pb_size_t, size_t>, TELEMETRY_MAX_SUBSCRIPTIONS> m_subscriptions;
};

}