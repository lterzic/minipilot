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

vector<rps_t, 3>
gyroscope_reader::process(const gyroscope::data_t& raw_data) noexcept
{
    vector<rps_t, 3> raw_vec {raw_data[0], raw_data[1], raw_data[2]};
    vector<rps_t, 3> ned_vec = m_transform.matmul(raw_vec);
    
    // TODO: Calculate drift from DC value by undersampling
    // based on the sensor rate (task period)
    m_drift.fill(rps_t(0));

    return ned_vec - m_drift;
}

}