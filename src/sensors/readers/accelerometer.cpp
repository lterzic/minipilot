#include "accelerometer.hpp"

namespace mp {

accelerometer_reader::accelerometer_reader(const accelerometer_config_s& config) :
    sensor_reader(
        config.device,
        TASK_ACCEL_PRIORITY,
        TASK_ACCEL_PERIOD
    ),
    m_transform(config.transform)
{}

bool
accelerometer_reader::init(sensor_t& sensor) noexcept
{
    // TODO: Get bias from the saved settings or if not available
    // warn the user and and ask to read multiple samples to calculate bias
    m_bias.fill(mpss_t(0));
    return true;
}

vector<mpss_t, 3>
accelerometer_reader::process(const accelerometer::data_t& raw_data) const noexcept
{
    // TODO: Add (optional) low-pass filtering
    
    // TODO: Update emblib so that casts don'd do lazy eval
    vector<mpss_t, 3> raw_vec {raw_data[0], raw_data[1], raw_data[2]};
    return m_transform.matmul(raw_vec - m_bias);
}

}