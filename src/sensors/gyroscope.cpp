#include "gyroscope.hpp"
#include "mp_config.hpp"

namespace mp {

gyroscope_reader::gyroscope_reader(const gyroscope_config_s& config) :
    sensor_reader(
        config.device,
        config.mutex,
        GYROSCOPE_READER_PRIORITY,
        GYROSCOPE_READER_PERIOD
    ),
    m_transform(config.transform)
{}

bool
gyroscope_reader::init(sensor_t& sensor) noexcept
{
    return true;
}

vector<emblib::rps_t, 3>
gyroscope_reader::process(const sensor_t::data_t& raw_data) noexcept
{
    vector<emblib::rps_t, 3> raw_vec {
        emblib::rps_t(raw_data[0]),
        emblib::rps_t(raw_data[1]),
        emblib::rps_t(raw_data[2])
    };
    vector<emblib::rps_t, 3> ned_vec = m_transform.matmul(raw_vec);
    
    // TODO: Calculate drift from DC value by undersampling
    // based on the sensor rate (task period)
    m_drift.fill(emblib::rps_t(0));

    return ned_vec - m_drift;
}

}