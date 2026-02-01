#include "sensors/gyroscope.hpp"
#include "common/pb.hpp"

namespace mp {

gyroscope_processor::gyroscope_processor(gyroscope& gyroscope) :
    sensor_processor(gyroscope, GYROSCOPE_PROCESSOR_PRIORITY)
{}

bool
gyroscope_processor::produce(payload_u& payload) noexcept
{
    payload.gyroscope.has_angular_velocity = true;
    payload.gyroscope.angular_velocity = pb_from(read().first.cast<float>());
    payload.gyroscope.has_raw_angular_velocity = true;
    payload.gyroscope.raw_angular_velocity = pb_from(read_raw().first.cast<float>());
    return true;
}

gyroscope_data_type
gyroscope_processor::process(const gyroscope_data_type& raw_data) noexcept
{
    return raw_data;
}

}