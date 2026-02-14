#pragma once

#include "common/chrono.hpp"
#include "producer_registry.hpp"
#include <emblib/rtos/task.hpp>
#include <etl/set.h>
#include <pblink/link/modules/telemetry.nanopb.h>
#include <pblink/telemetry/telemetry.nanopb.h>

namespace mp {

/**
 * Sink represents a single telemetry output path with
 * its own subscriptions and telemetry message generation.
 */
class telemetry_sink : private emblib::rtos::static_task<1024> {
public:
    /**
     * Producer ID consists of a channel and source index to be able
     * to differentiate between multiple produces of the same channel type.
     */
    struct producer_id {
        telemetry_channel_e channel;
        size_t source_idx;

        bool operator<(const producer_id& rhs) const { return channel < rhs.channel; }
    };

public:
    /**
     * Create a sink with a specified message creation period.
     */
    explicit telemetry_sink(milliseconds_t period);
    virtual ~telemetry_sink() = default;

    /**
     * Subscribe to a telemetry producer.
     */
    bool subscribe(producer_id producer) noexcept;

private:
    /**
     * Emit a constructed telemetry message.
     */
    virtual bool write(const pblink::telemetry::telemetry_s& msg) noexcept = 0;

    /**
     * Periodically fetch all producers based on current subscriptions
     * and generate a telemetry message which is forwarded to `write`.
     */
    void run() noexcept override;

private:
    static inline constexpr auto MAX_CHANNELS =
        sizeof(pblink::telemetry::telemetry_s::channels) /
        sizeof(pblink::telemetry::channel_s);

    milliseconds_t m_period;
    etl::set<producer_id, MAX_CHANNELS> m_subscriptions;
};

}