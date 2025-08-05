#include "rx_handler_command.hpp"

namespace mp {

rx_handler_command::rx_handler_command(copter_control* copter) noexcept :
    m_copter(copter)
{}

static void handle_copter(copter_control* copter, const pb_mp_CopterCommand& cmd)
{
    switch (cmd.which_command_type) {
        case pb_mp_CopterCommand_set_angular_velocity_tag: {
            const auto& w = cmd.command_type.set_angular_velocity.angular_velocity;
            const float thrust = cmd.command_type.set_angular_velocity.thrust;
            copter->set_angular_velocity({w.x, w.y, w.z}, thrust);
            // TODO: Handle return value
            break;
        }
        case pb_mp_CopterCommand_set_linear_velocity_tag: {
            const auto& v = cmd.command_type.set_linear_velocity.velocity;
            const float dir = cmd.command_type.set_linear_velocity.direction;
            copter->set_linear_velocity({v.x, v.y, v.z}, dir);
            // TODO: Handle return value
            break;
        }
        default:
            break;
    }
}

void rx_handler_command::handle(const pb_mp_UplinkMessage& msg) noexcept
{
    const pb_mp_Command& command = msg.payload.command;
    switch (command.which_vehicle) {
    case pb_mp_Command_copter_tag:
        if (m_copter)
            handle_copter(m_copter, command.vehicle.copter);
        break;
    default:
        break;
    }
}

}