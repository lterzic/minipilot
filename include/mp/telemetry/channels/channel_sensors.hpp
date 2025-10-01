#pragma once

#include "sensors/sensor_manager.hpp"
#include "state/state_estimator.hpp"
#include "telemetry/telemetry.hpp"

namespace mp {

class channel_sensors {
public:
    channel_sensors(
        telemetry& telemetry,
        const sensor_manager& sensor_manager,
        const state_estimator& state_estimator
    );

private:
    void set(telemetry::channel_payload_u& channel) const;

private:
    const sensor_manager& m_sensor_manager;
    const state_estimator& m_state_estimator;
};

}