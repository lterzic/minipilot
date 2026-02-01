#pragma once

#include "common/config.hpp"
#include "link/handler.hpp"
#include "link/tx.hpp"
#include "producer.hpp"
#include <emblib/rtos/task.hpp>
#include <etl/set.h>
#include <etl/unordered_map.h>
#include <etl/vector.h>

namespace mp {

class telemetry :
    public rx_handler<rx_payload_e::TELEMETRY>,
    private emblib::rtos::static_task<1024> {
    /**
     * Telemetry producer template parameter is not used when
     * calling methods, so we can store all producers with the
     * same parameter value
     */
    using telemetry_producer_default = telemetry_producer<telemetry_channel_e::ACCELEROMETER>;

public:
    explicit telemetry(tx& link);

    /**
     * Add a subscriber for a specific channel
     * @todo Add frequency as a parameter
     */
    bool add_subscriber(telemetry_channel_e channel, size_t producer_id) noexcept;

    /**
     * Add a channel producer
     */
    template <telemetry_channel_e CHANNEL>
    bool add_producer(telemetry_producer<CHANNEL>& producer) noexcept
    {
        return add_producer(CHANNEL, reinterpret_cast<telemetry_producer_default&>(producer));
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
     * Producer template parameter is not used at runtime currently, so
     * public add_producer calls are forwarded to this
     */
    bool add_producer(telemetry_channel_e channel, telemetry_producer_default& producer) noexcept;

    /**
     * Encoder for telemetry data
     * @param arg is set to `this`
     */
    static bool encode_telemetry(pb_ostream_t* stream, const pb_field_t*, void* const* arg) noexcept;

    /**
     * Encoder for broadcast data
     * @param arg is set to `this`
     */
    static bool encode_broadcast(pb_ostream_t* stream, const pb_field_t*, void* const* arg) noexcept;

    /**
     * Handle rx messages
     */
    void handle(payload_u& payload) noexcept override;

private:
    // Reference to the link transmitter
    tx& m_tx;
    // Map channel types to a vector of channel sources
    etl::unordered_map<telemetry_channel_e, etl::vector<telemetry_producer_default*, 1>, pb::telemetry::channel_s::PAYLOAD_COUNT> m_producers;
    // List of (channel tag, channel source id) subscriptions
    etl::set<etl::pair<telemetry_channel_e, size_t>, TELEMETRY_MAX_SUBSCRIPTIONS> m_subscriptions;
};

}