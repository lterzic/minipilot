#include "sensors/accelerometer.hpp"
#include "common/pb.hpp"

namespace mp {

accelerometer_processor::accelerometer_processor(accelerometer& accelerometer) :
    sensor_processor(accelerometer, ACCELEROMETER_PROCESSOR_PRIORITY)
{
    m_bias.fill(emblib::units::meters_per_second_squared<float>(0));
}

bool
accelerometer_processor::produce(payload_u& payload) noexcept
{
    payload.accelerometer.has_acceleration = true;
    payload.accelerometer.acceleration = pb_from(read().first.cast<float>());
    payload.accelerometer.has_raw_acceleration = true;
    payload.accelerometer.raw_acceleration = pb_from(read_raw().first.cast<float>());
    return true;
}

accelerometer_data_type
accelerometer_processor::process(const accelerometer_data_type& raw_data) noexcept
{
    return raw_data - m_bias;
}

}