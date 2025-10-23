#pragma once

#include "state/state_estimator_task.hpp"
#include "telemetry/telemetry.hpp"

namespace mp {

class channel_state {
public:
    channel_state(
        telemetry& telemetry,
        const state_estimator_task& state_estimator_task
    );

private:
    void set(telemetry::channel_payload_u& channel) const;

private:
    const state_estimator_task& m_state_estimator_task;
};

}