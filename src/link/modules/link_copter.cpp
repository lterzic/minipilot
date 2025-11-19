#include "link/modules/link_copter.hpp"
#include "common/pb.hpp"
#include <pb_encode.h>

namespace mp {

link_copter::link_copter(link& link, copter_controller_task& controller) :
    m_controller(controller)
{
    assert(link.m_rx.set_handler(mp_pb_link_Uplink_copter_tag, *this));
}

void
link_copter::handle(payload_u& payload) noexcept
{
    mp_pb_link_CopterUplink& msg = payload.copter;

    switch (msg.which_payload) {
    case mp_pb_link_CopterUplink_set_angular_velocity_tag:
        // If failed to set, can return false response or log a warning
        m_controller.set_angular_controls({
            .angular_velocity = pb_vector3f(msg.payload.set_angular_velocity.angular_velocity),
            .thrust = msg.payload.set_angular_velocity.thrust
        });
        break;
    case mp_pb_link_CopterUplink_set_linear_velocity_tag:
        m_controller.set_linear_controls({
            .linear_velocity = pb_vector3f(msg.payload.set_linear_velocity.linear_velocity),
            .direction = msg.payload.set_linear_velocity.direction
        });
        break;
    default:
        break;
    }
}

}