#include "link/modules/link_telemetry.hpp"

namespace mp {

link_telemetry::link_telemetry(link& link, telemetry& telemetry) :
    m_telemtry(telemetry)
{
    assert(link.m_rx.set_handler(pb::link::uplink_s::payload_e::TELEMETRY, *this));
}

void
link_telemetry::handle(payload_u& payload) noexcept
{
    pb::link::telemetry_uplink_s& msg = payload.telemetry;

    switch (msg.which_payload) {
    case pb::link::telemetry_uplink_s::payload_e::SUBSCRIBE:
        m_telemtry.add_subscriber(
            telemetry::channel_e(msg.payload.subscribe.channel),
            msg.payload.subscribe.source
        );
        break;
    default:
        break;
    }
}

}