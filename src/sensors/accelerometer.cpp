#include "sensors/accelerometer.hpp"
#include "common/pb.hpp"

namespace mp {

accelerometer_processor::accelerometer_processor(accelerometer& accelerometer) :
    sensor_processor(accelerometer, ACCELEROMETER_PROCESSOR_PRIORITY)
{
    m_bias.fill(emblib::units::meters_per_second_squared<float>(0));
}

bool
accelerometer_processor::produce(payload_u& payload) const noexcept
{
    PB_SET(payload.accelerometer, acceleration, read().first.cast<float>());
    PB_SET(payload.accelerometer, raw_acceleration, read_raw().first.cast<float>());
    return true;
}

accelerometer_data_type
accelerometer_processor::process(const accelerometer_data_type& raw_data) noexcept
{
    return raw_data - m_bias;
}

}