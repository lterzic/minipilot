#pragma once

#include "link/telemetry.hpp"
#include "state/state_estimator_task.hpp"

namespace mp {

class telemetry_state : private telemetry_producer {
public:
    explicit telemetry_state(
        telemetry& telemetry,
        const state_estimator_task& estimator
    );

private:
    bool produce(payload_u& payload) const noexcept override;
    
private:
    const state_estimator_task& m_estimator;
};

}