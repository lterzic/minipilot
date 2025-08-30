#include "sensors/gyroscope.hpp"
#include "common/config.hpp"

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

vector<gyroscope_units, 3>
gyroscope_reader::process(const sensor_t::data_t& raw_data) noexcept
{
    vector<gyroscope_units, 3> raw_vec {
        gyroscope_units(raw_data[0]),
        gyroscope_units(raw_data[1]),
        gyroscope_units(raw_data[2])
    };
    vector<gyroscope_units, 3> aligned_vec = m_transform.matmul(raw_vec);
    
    return aligned_vec;
}

}