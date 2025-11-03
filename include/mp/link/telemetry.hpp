#pragma once

#include "common/config.hpp"
#include "link/tx.hpp"
#include <emblib/rtos/task.hpp>
#include <etl/unordered_map.h>
#include <etl/set.h>
#include <etl/vector.h>
#include <pb_encode.h>

namespace mp {

template <pb_size_t PAYLOAD_TYPE>
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
    virtual bool set_telemetry(payload_u& payload) const noexcept = 0;
};

class telemetry : private emblib::rtos::static_task<1024> {
public:
    /**
     * Map size should be equal to (or at least greater than) the number of channel types
     * Currently only allowing one source per channel type
     * @note Template type of the producer is not relevant here as its not used for the method
     */
    using channel_map = etl::unordered_map<pb_size_t, etl::vector<telemetry_producer<0>*, 1>, 4>;

public:
    explicit telemetry(tx& tx);

    /**
     * Add a subscriber for a specific channel
     * @todo Add frequency as a parameter
     */
    bool add_subscription(pb_size_t channel_type, size_t channel_source) noexcept
    {
        if (m_subscriptions.full())
            return false;
        m_subscriptions.insert({channel_type, channel_source});
        return true;
    }

    /**
     * Add a channel source
     */
    template <pb_size_t CHANNEL_TYPE>
    bool add_producer(telemetry_producer<CHANNEL_TYPE>* producer) noexcept
    {
        if (m_channels[CHANNEL_TYPE].full())
            return false;
        m_channels[CHANNEL_TYPE].push_back(producer);
        return true;
    }

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