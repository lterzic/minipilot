#include "state/state_telemetry.hpp"
#include "common/pb.hpp"

namespace mp {

state_telemetry::state_telemetry(
    const state_estimator& state_estimator,
    telemetry& telemetry
) :
    m_state_estimator(state_estimator),
    m_telemetry(telemetry)
{
    assert(m_telemetry.add_channel(
        mp_pb_telemetry_Channel_state_tag,
        etl::make_delegate<state_telemetry, &state_telemetry::channel_state>(*this)
    ));
}

void
state_telemetry::channel_state(telemetry::channel_payload_u& channel) const noexcept
{
    state_s state = m_state_estimator.get_state();
    PB_SET(channel.state, position, state.position);
    PB_SET(channel.state, velocity, state.velocity);
    PB_SET(channel.state, acceleration, state.acceleration);
    PB_SET(channel.state, angular_velocity, state.angular_velocity);
    PB_SET(channel.state, rotation, state.rotationq.as_vector());
}

}