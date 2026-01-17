#include "common/config.hpp"
#include "telemetry/telemetry.hpp"
#include <pb_encode.h>

namespace mp {

telemetry::telemetry(link& link) :
    static_task("telemetry", TELEMETRY_TASK_PRIORITY),
    m_tx(link.m_tx)
{}

bool
telemetry::add_subscriber(telemetry_channel_e channel, size_t producer_id) noexcept
{
    return m_subscriptions.insert({channel, producer_id}).second;
}

bool
telemetry::add_producer(telemetry_channel_e channel, telemetry_producer_default& producer) noexcept
{
    if (m_producers.find(channel) == m_producers.end())
        return false;
    
    if (m_producers[channel].full())
        return false;
    
    m_producers[channel].push_back(&producer);
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
        broadcast_msg.channels.arg = (void*)this;
        broadcast_msg.channels.funcs.encode = &telemetry::encode_broadcast;
    });
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
            telemetry_msg.channels.funcs.encode = &telemetry::encode_telemetry;
        });

        sleep_periodic(TELEMETRY_PERIOD);
    }
}

bool
telemetry::encode_telemetry(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) noexcept
{
    const auto* telemetry_obj = static_cast<telemetry*>(*arg);

    for (auto [channel_tag, producer_id] : telemetry_obj->m_subscriptions) {
        pb::telemetry::channel_s channel = {0};
        channel.source_id = producer_id;
        channel.which_payload = channel_tag;
        telemetry_obj->m_producers.at(channel_tag)[producer_id]->produce(channel.payload);

        if (!pb_encode_tag_for_field(stream, field))
            return false;
        if (!pb_encode_submessage(stream, MP_PB_TELEMETRY_CHANNEL_FIELDS, &channel))
            return false;
    }
    return true;
}

bool
telemetry::encode_broadcast(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) noexcept
{
    const auto* telemetry_obj = static_cast<telemetry*>(*arg);

    for (const auto& [channel_tag, producers] : telemetry_obj->m_producers) {
        for (size_t producer_id = 0; producer_id < producers.size(); producer_id++) {
            pb::telemetry::channel_s channel = {0};
            channel.source_id = producer_id;
            channel.which_payload = channel_tag;
            producers[producer_id]->broadcast(channel.payload);
            // TODO: encode name from the producer

            if (!pb_encode_tag_for_field(stream, field))
                return false;
            if (!pb_encode_submessage(stream, MP_PB_TELEMETRY_CHANNEL_FIELDS, &channel))
                return false;
        }
    }
    return true;
}

}