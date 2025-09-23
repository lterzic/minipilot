#pragma once

#include "common/config.hpp"
#include "link/tx.hpp"
#include "telemetry/telemetry_producer.hpp"
#include <emblib/rtos/task.hpp>
#include <etl/unordered_map.h>
#include <etl/vector.h>
#include <pb_encode.h>

namespace mp {

class telemetry : private emblib::rtos::static_task<1024> {
public:
    /**
     * Array of producers (sources) for each channel type
     */
    using producer_map_t = etl::iunordered_map<pb_size_t, etl::ivector<telemetry_producer*>*>;

public:
    explicit telemetry(
        tx& tx,
        producer_map_t& producers
    );

private:
    /**
     * Add a subscriber for a specific channel
     * @todo Add frequency as a parameter
     */
    bool add_subscription(pb_size_t channel_type, size_t channel_source) noexcept;

    /**
     * Send the list of producers over the link
     */
    void broadcast() const noexcept;

    /**
     * Periodically emit all channels which have a subscription
     */
    void run() noexcept override;

    /**
     * Encoder for telemetry data. Part of the class to provide access
     * to producer and subscription members
     */
    static bool encode_channels(pb_ostream_t* stream, const pb_field_t*, void* const* arg) noexcept;

private:
    tx& m_tx;
    producer_map_t& m_producers;
    // List of (channel tag, channel source id) subscriptions
    etl::vector<etl::pair<pb_size_t, size_t>, TELEMETRY_MAX_SUBSCRIPTIONS> m_subscriptions;
};

}