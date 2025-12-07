#include "common/config.hpp"
#include "telemetry/telemetry.hpp"
#include <pb_encode.h>

namespace mp {

telemetry::telemetry(link& link) :
    static_task("telemetry", TELEMETRY_TASK_PRIORITY),
    m_tx(link.m_tx)
{}

bool
telemetry::add_subscriber(channel_e channel, size_t producer_id) noexcept
{
    if (m_subscriptions.full())
        return false;
    
    m_subscriptions.insert({channel, producer_id});
    return true;
}

bool
telemetry::add_producer(channel_e channel, telemetry_producer& producer) noexcept
{
    if (m_channels[channel].full())
        return false;
    
    m_channels[channel].push_back(&producer);
    return true;
}

static bool
encode_channel_sources(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) noexcept
{
    const auto* map = static_cast<telemetry::channel_map*>(*arg);

    for (const auto& [channel, producers] : *map) {
        pb::telemetry::channel_sources_s channel_sources {
            .channel = static_cast<uint32_t>(channel),
            .sources = static_cast<uint32_t>(producers.size())
        };
        
        if (!pb_encode_tag_for_field(stream, field))
            return false;
        if (!pb_encode_submessage(stream, MP_PB_TELEMETRY_BROADCAST_CHANNEL_SOURCES_FIELDS, &channel_sources))
            return false;
    }
    return true;
}

void
telemetry::broadcast() const noexcept
{
    m_tx.send([this](tx::tx_message_s& msg) {
        msg.which_payload = pb::link::downlink_s::payload_e::TELEMETRY;
        pb::link::telemetry_downlink_s& telemetry_downlink = msg.payload.telemetry;

        telemetry_downlink.which_payload = pb::link::telemetry_downlink_s::payload_e::BROADCAST;
        pb::telemetry::broadcast_s& broadcast_msg = telemetry_downlink.payload.broadcast;

        // Here the constness of the method is ignored, but we know that the
        // encode function does not modify the map.
        // TODO: Make the channels pre-allocated so that they would be collected
        // at the time of this function being called instead of time of encoding.
        broadcast_msg.channels.arg = (void*)&m_channels;
        broadcast_msg.channels.funcs.encode = &encode_channel_sources;
    });
}

bool
telemetry::encode_channels(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) noexcept
{
    auto* instance = static_cast<telemetry*>(*arg);

    for (auto [channel_tag, producer_id] : instance->m_subscriptions) {
        pb::telemetry::channel_s channel = {0};
        channel.source = producer_id;
        channel.which_payload = channel_tag;
        instance->m_channels[channel_tag][producer_id]->produce(channel.payload);

        if (!pb_encode_tag_for_field(stream, field))
            return false;
        if (!pb_encode_submessage(stream, MP_PB_TELEMETRY_CHANNEL_FIELDS, &channel))
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
        m_tx.send([this](tx::tx_message_s& msg) {
            msg.which_payload = pb::link::downlink_s::payload_e::TELEMETRY;
            pb::link::telemetry_downlink_s& telemetry_downlink = msg.payload.telemetry;

            telemetry_downlink.which_payload = pb::link::telemetry_downlink_s::payload_e::TELEMETRY;
            pb::telemetry::telemetry_s& telemetry_msg = telemetry_downlink.payload.telemetry;

            // TODO: Make the channels pre-allocated so that they would be collected
            // at the time of this function being called instead of time of encoding,
            // and to avoid data races since telemetry and tx are different threads.
            telemetry_msg.timestamp_ms = get_time_since_start().value();
            telemetry_msg.channels.arg = this;
            telemetry_msg.channels.funcs.encode = &telemetry::encode_channels;
        });

        sleep_periodic(TELEMETRY_PERIOD);
    }
}

}