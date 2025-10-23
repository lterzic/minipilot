#include "telemetry/channels/channel_state.hpp"
#include "common/pb.hpp"

namespace mp {

channel_state::channel_state(
    telemetry& telemetry,
    const state_estimator_task& state_estimator_task
) :
    m_state_estimator_task(state_estimator_task)
{
    assert(telemetry.add_channel(
        mp_pb_telemetry_Channel_state_tag,
        etl::make_delegate<channel_state, &channel_state::set>(*this)
    ));
}

void
channel_state::set(telemetry::channel_payload_u& channel) const noexcept
{
    state_s state = m_state_estimator_task.get_state();
    PB_SET(channel.state, position, state.position);
    PB_SET(channel.state, velocity, state.velocity);
    PB_SET(channel.state, acceleration, state.acceleration);
    PB_SET(channel.state, angular_velocity, state.angular_velocity);
    PB_SET(channel.state, rotation, state.rotationq.as_vector());
}

}