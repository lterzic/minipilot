#include "common/config.hpp"
#include "telemetry/telemetry.hpp"
#include <pb_encode.h>

namespace mp {

telemetry::telemetry(tx& tx) :
    static_task("Telemetry", TELEMETRY_TASK_PRIORITY),
    m_tx(tx)
{}

bool
telemetry::add_channel(pb_size_t channel_type, channel_cb cb) noexcept
{
    if (m_channels[channel_type].full())
        return false;
    m_channels[channel_type].push_back(cb);
    return true;
}

bool
telemetry::add_subscription(pb_size_t channel_type, size_t channel_source) noexcept
{
    if (m_subscriptions.full())
        return false;
    m_subscriptions.insert({channel_type, channel_source});
    return true;
}

static bool
encode_channel_sources(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) noexcept
{
    const auto* map = static_cast<telemetry::channel_map*>(*arg);

    for (const auto& [ch, channels] : *map) {
        mp_pb_telemetry_Downlink_Broadcast_ChannelSources channel_sources {
            .channel = ch,
            .sources = static_cast<uint32_t>(channels.size())
        };
        
        if (!pb_encode_tag_for_field(stream, field))
            return false;
        if (!pb_encode_submessage(stream, mp_pb_telemetry_Downlink_Broadcast_ChannelSources_fields, &channel_sources))
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

        // Here the constness of the method is ignored, but we know that the
        // encode function does not modify the map
        broadcast.channels.arg = (void*)&m_channels;
        broadcast.channels.funcs.encode = &encode_channel_sources;
    };
    m_tx.send_downlink<mp_pb_telemetry_Downlink>(cb);
}

bool
telemetry::encode_channels(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) noexcept
{
    auto* instance = static_cast<telemetry*>(*arg);

    for (auto [ch, source] : instance->m_subscriptions) {
        mp_pb_telemetry_Channel channel;
        channel.source = source;
        channel.which_payload = ch;
        instance->m_channels[ch][source](channel.payload);

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