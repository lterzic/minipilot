#include "accelerometer.hpp"
#include "mp_config.hpp"

namespace mp {

accelerometer_reader::accelerometer_reader(const accelerometer_config_s& config) :
    sensor_reader(
        config.device,
        config.mutex,
        ACCELEROMETER_READER_PRIORITY,
        ACCELEROMETER_READER_PERIOD
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
accelerometer_reader::process(const accelerometer::data_t& raw_data) noexcept
{
    // TODO: Update emblib so that casts don'd do lazy eval
    vector<mpss_t, 3> raw_vec {raw_data[0], raw_data[1], raw_data[2]};
    vector<mpss_t, 3> ned_vec = m_transform.matmul(raw_vec);
    
    // TODO: Add (optional) low-pass filtering
    return ned_vec - m_bias;
}

}