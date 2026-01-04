#include "link/modules/link_telemetry.hpp"

namespace mp {

link_telemetry::link_telemetry(link& link, telemetry& telemetry) :
    m_telemtry(telemetry)
{
    assert(link.m_rx.set_handler(*this));
}

void
link_telemetry::handle(payload_u& payload) noexcept
{
    pb::link::telemetry_uplink_s& msg = payload.telemetry;

    switch (msg.which_payload) {
    case pb::link::telemetry_uplink_s::payload_e::SUBSCRIBE:
        m_telemtry.add_subscriber(
            telemetry_channel_e(msg.payload.subscribe.channel),
            msg.payload.subscribe.source_id
        );
        break;
    default:
        break;
    }
}

}