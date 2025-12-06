#pragma once

#include "telemetry/telemetry.hpp"
#include "sensors/sensor_manager.hpp"
#include "state/state_estimator_task.hpp"

namespace mp {

class producer_sensors : private telemetry_producer {
public:
    explicit producer_sensors(
        telemetry& telemetry,
        const sensor_manager& sensor_manager,
        const state_estimator_task& estimator
    );

private:
    bool produce(payload_u& payload) const noexcept override;
    
private:
    const sensor_manager& m_sensor_manager;
    const state_estimator_task& m_estimator;
};

}