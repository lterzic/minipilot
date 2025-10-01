#pragma once

#include "state/state_estimator.hpp"
#include "telemetry/telemetry.hpp"

namespace mp {

class channel_state {
public:
    channel_state(
        telemetry& telemetry,
        const state_estimator& state_estimator
    );

private:
    void set(telemetry::channel_payload_u& channel) const;

private:
    const state_estimator& m_state_estimator;
};

}