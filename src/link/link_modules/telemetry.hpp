#pragma once

#include "link/tx.hpp"
#include "sensors/sensor_manager.hpp"
#include "state/state_estimator.hpp"
#include "emblib/rtos/task.hpp"

namespace mp {

class telemetry : private emblib::rtos::static_task<1024> {

public:
    explicit telemetry(
        tx& tx,
        const sensor_manager& sensor_manager,
        const state_estimator& state_estimator
    );

private:
    void run() noexcept override;

private:
    tx& m_tx;

    const sensor_manager& m_sensor_manager;
    const state_estimator& m_state_estimator;
};

}