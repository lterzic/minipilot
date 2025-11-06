#include "common/config.hpp"
#include "link/telemetry.hpp"
#include <pb_encode.h>

namespace mp {

telemetry::telemetry(link& link) :
    static_task("Telemetry", TELEMETRY_TASK_PRIORITY),
    m_tx(link.m_tx)
{}

bool
telemetry::add_subscriber(pb_size_t channel_tag, size_t producer_id) noexcept
{
    if (m_subscriptions.full())
        return false;
    
    m_subscriptions.insert({channel_tag, producer_id});
    return true;
}

bool
telemetry::add_producer(pb_size_t channel_tag, telemetry_producer& producer) noexcept
{
    if (m_channels[channel_tag].full())
        return false;
    
    m_channels[channel_tag].push_back(&producer);
    return true;
}

static bool
encode_channel_sources(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) noexcept
{
    const auto* map = static_cast<telemetry::channel_map*>(*arg);

    for (const auto& [channel_tag, producers] : *map) {
        mp_pb_telemetry_Downlink_Broadcast_ChannelSources channel_sources {
            .channel = channel_tag,
            .sources = static_cast<uint32_t>(producers.size())
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
    mp_pb_link_Downlink msg = {0};
    msg.which_payload = mp_pb_link_Downlink_telemetry_tag;

    mp_pb_telemetry_Downlink& telemetry_downlink = msg.payload.telemetry;
    telemetry_downlink.which_payload = mp_pb_telemetry_Downlink_broadcast_tag;

    mp_pb_telemetry_Downlink_Broadcast& broadcast_msg = telemetry_downlink.payload.broadcast;
    // Here the constness of the method is ignored, but we know that the
    // encode function does not modify the map
    broadcast_msg.channels.arg = (void*)&m_channels;
    broadcast_msg.channels.funcs.encode = &encode_channel_sources;
    
    m_tx.send_downlink(msg);
}

bool
telemetry::encode_channels(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) noexcept
{
    auto* instance = static_cast<telemetry*>(*arg);

    for (auto [channel_tag, producer_id] : instance->m_subscriptions) {
        mp_pb_telemetry_Channel channel = {0};
        channel.source = producer_id;
        channel.which_payload = channel_tag;
        instance->m_channels[channel_tag][producer_id]->produce(channel.payload);

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
    sleep(milliseconds_t(10));

    while (true) {
        mp_pb_link_Downlink msg = {0};
        msg.which_payload = mp_pb_link_Downlink_telemetry_tag;

        mp_pb_telemetry_Downlink& telemetry_downlink = msg.payload.telemetry;
        telemetry_downlink.which_payload = mp_pb_telemetry_Downlink_telemetry_tag;

        mp_pb_telemetry_Downlink_Telemetry& telemetry_msg = telemetry_downlink.payload.telemetry;
        telemetry_msg.timestamp_ms = get_time_since_start().value();
        telemetry_msg.channels.arg = this;
        telemetry_msg.channels.funcs.encode = &telemetry::encode_channels;

        m_tx.send_downlink(msg);

        sleep_periodic(TELEMETRY_PERIOD);
    }
}

}