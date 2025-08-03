#pragma once

#include "pb_tx.hpp"
#include "sensors/sensor_manager.hpp"
#include "state/state_estimator.hpp"
#include "emblib/rtos/task.hpp"

namespace mp {

class telemetry : public emblib::task_static<1024> {

public:
    explicit telemetry(
        pb_tx& tx,
        const sensor_manager& sensor_manager,
        const state_estimator& state_estimator
    );

private:
    void run() noexcept override;

private:
    pb_tx& m_tx;

    const sensor_manager& m_sensor_manager;
    const state_estimator& m_state_estimator;
};

}