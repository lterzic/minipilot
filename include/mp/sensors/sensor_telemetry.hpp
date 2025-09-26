#pragma once

#include "sensor_manager.hpp"
#include "state/state_estimator.hpp"
#include "telemetry/telemetry.hpp"

namespace mp {

class sensor_telemetry {
public:
    sensor_telemetry(
        const sensor_manager& sensor_manager,
        const state_estimator& state_estimator,
        telemetry& telemetry
    );

private:
    void channel_sensors(telemetry::channel_payload_u& channel) const;

private:
    const sensor_manager& m_sensor_manager;
    const state_estimator& m_state_estimator;
    telemetry& m_telemetry;
};

}