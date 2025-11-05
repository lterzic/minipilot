#include "link/handlers/handler_copter.hpp"
#include "common/pb.hpp"

namespace mp {

handle_copter::handle_copter(rx& rx, copter_controller_task& controller) :
    m_controller(controller)
{
    assert(rx.set_handler(
        mp_pb_link_Uplink_copter_tag,
        etl::make_delegate<handle_copter, &handle_copter::handle>(*this)
    ));
}

void
handle_copter::handle(const mp_pb_link_Uplink& msg) noexcept
{
    auto& copter_msg = msg.payload.copter;
    switch (copter_msg.which_payload) {
    case mp_pb_copter_Uplink_set_angular_velocity_tag:
        // If failed to set, can return false response or log a warning
        m_controller.set_angular_controls({
            .angular_velocity = pb_vector3f(copter_msg.payload.set_angular_velocity.angular_velocity),
            .thrust = copter_msg.payload.set_angular_velocity.thrust
        });
        break;
    case mp_pb_copter_Uplink_set_linear_velocity_tag:
        m_controller.set_linear_controls({
            .linear_velocity = pb_vector3f(copter_msg.payload.set_linear_velocity.linear_velocity),
            .direction = copter_msg.payload.set_linear_velocity.direction
        });
        break;
    default:
        break;
    }
}

}