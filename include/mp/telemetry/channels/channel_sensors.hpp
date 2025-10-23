#pragma once

#include "sensors/sensor_manager.hpp"
#include "state/state_estimator_task.hpp"
#include "telemetry/telemetry.hpp"

namespace mp {

class channel_sensors {
public:
    channel_sensors(
        telemetry& telemetry,
        const sensor_manager& sensor_manager,
        const state_estimator_task& state_estimator_task
    );

private:
    void set(telemetry::channel_payload_u& channel) const;

private:
    const sensor_manager& m_sensor_manager;
    const state_estimator_task& m_state_estimator_task;
};

}