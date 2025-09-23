#include "common/config.hpp"
#include "telemetry/telemetry.hpp"
#include <pb_encode.h>

namespace mp {

telemetry::telemetry(
    tx& tx,
    producer_map_t& producers
) :
    static_task("Telemetry", TELEMETRY_TASK_PRIORITY),
    m_tx(tx),
    m_producers(producers)
{}

bool
telemetry::add_subscription(pb_size_t channel_type, size_t channel_source) noexcept
{
    if (m_subscriptions.full())
        return false;
    m_subscriptions.push_back({channel_type, channel_source});
    return true;
}

static bool
encode_channel_sources(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) noexcept
{
    const auto* map = static_cast<telemetry::producer_map_t*>(*arg);
    for (const auto& [ch, source_vector] : *map) {
        mp_pb_telemetry_Downlink_Broadcast_ChannelSources sources {
            .channel = ch,
            .sources = static_cast<uint32_t>(source_vector->size())
        };
        
        if (!pb_encode_tag_for_field(stream, field))
            return false;
        if (!pb_encode_submessage(stream, mp_pb_telemetry_Downlink_Broadcast_ChannelSources_fields, &sources))
            return false;
    }
    return true;
}

void
telemetry::broadcast() const noexcept
{
    auto cb = [this](mp_pb_telemetry_Downlink& downlink) {
        mp_pb_telemetry_Downlink_Broadcast& broadcast = downlink.payload.broadcast;
        downlink.which_payload = mp_pb_telemetry_Downlink_broadcast_tag;

        broadcast.channels.arg = &m_producers;
        broadcast.channels.funcs.encode = &encode_channel_sources;
    };
    m_tx.send_downlink<mp_pb_telemetry_Downlink>(cb);
}

bool
telemetry::encode_channels(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) noexcept
{
    const auto* instance = static_cast<telemetry*>(*arg);

    for (auto [ch, source] : instance->m_subscriptions) {
        mp_pb_telemetry_Channel channel;
        (*instance->m_producers[ch])[source]->fill_channel(channel);

        if (!pb_encode_tag_for_field(stream, field))
            return false;
        if (!pb_encode_submessage(stream, mp_pb_telemetry_Channel_fields, &channel))
            return false;
    }
    return true;
}

void
telemetry::run() noexcept
{
    // Do an initial broadcast
    broadcast();

    while (true) {
        auto cb = [this](mp_pb_telemetry_Downlink& downlink) {
            mp_pb_telemetry_Downlink_Telemetry& telemetry = downlink.payload.telemetry;
            downlink.which_payload = mp_pb_telemetry_Downlink_telemetry_tag;

            telemetry.timestamp_ms = get_time_since_start().value();
            telemetry.channels.arg = this;
            telemetry.channels.funcs.encode = &telemetry::encode_channels;
        };
        m_tx.send_downlink<mp_pb_telemetry_Downlink>(cb);

        sleep_periodic(TELEMETRY_PERIOD);
    }
}

}