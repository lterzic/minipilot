#include "sensors/sensor_manager.hpp"

namespace mp {

sensor_manager::sensor_manager(const sensors_s& sensors, telemetry& telemetry)
{
    if (sensors.m_accelerometer) {
        m_accelerometer.emplace(*sensors.m_accelerometer);
        telemetry.add_producer(*m_accelerometer);
    }

    if (sensors.m_gyroscope) {
        m_gyroscope.emplace(*sensors.m_gyroscope);
        telemetry.add_producer(*m_gyroscope);
    }
}

}