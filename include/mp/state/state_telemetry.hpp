#pragma once

#include "state/state_estimator.hpp"
#include "telemetry/telemetry.hpp"

namespace mp {

class state_telemetry {
public:
    state_telemetry(
        const state_estimator& state_estimator,
        telemetry& telemetry
    );

private:
    void channel_state(telemetry::channel_payload_u& channel) const;

private:
    const state_estimator& m_state_estimator;
    telemetry& m_telemetry;
};

}