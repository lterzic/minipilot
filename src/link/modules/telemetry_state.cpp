#include "link/modules/telemetry_state.hpp"
#include "common/pb.hpp"

namespace mp {

telemetry_state::telemetry_state(telemetry& telemetry, const state_estimator_task& estimator) :
    m_estimator(estimator)
{
    assert(telemetry.add_producer(pb::telemetry::channel_s::payload_e::STATE, *this));
}

bool
telemetry_state::produce(payload_u& payload) const noexcept
{
    state_s state = m_estimator.get_state();
    PB_SET(payload.state, position, state.position);
    PB_SET(payload.state, velocity, state.velocity);
    PB_SET(payload.state, acceleration, state.acceleration);
    PB_SET(payload.state, angular_velocity, state.angular_velocity);
    PB_SET(payload.state, rotation, state.rotation.as_vector());
    return true;
}

}