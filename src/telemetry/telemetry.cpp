#include "common/config.hpp"
#include "telemetry/telemetry.hpp"

namespace mp {

telemetry::telemetry(
    tx& tx,
    const sensor_manager& sensor_manager,
    const state_estimator& state_estimator
) :
    static_task("Telemetry", TELEMETRY_TASK_PRIORITY),
    m_tx(tx),
    m_sensor_manager(sensor_manager),
    m_state_estimator(state_estimator)
{}

void telemetry::run() noexcept
{
    while (true) {
        // TODO: Encode telemetry for each producer and tx.send_downlink

        sleep_periodic(TELEMETRY_PERIOD);
    }
}

}