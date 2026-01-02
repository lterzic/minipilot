#pragma once

#include "common/math.hpp"
#include "sensor.hpp"
#include "sensor_processor.hpp"
#include "telemetry/producer.hpp"
#include <emblib/units/rotation.hpp>

namespace mp {

/**
 * 3D vector in units of rad/s
 */
using gyroscope_data_type = vector<emblib::units::radians_per_second<float>, 3>;

/**
 * Gyroscope is a sensor which produces readings of the body angular
 * velocity in the local FRD frame.
 */
using gyroscope = sensor<gyroscope_data_type>;

/**
 * Gyroscope data processor
 */
class gyroscope_processor :
    public sensor_processor<gyroscope>,
    public telemetry_producer<telemetry_channel_e::GYROSCOPE> {
public:
    /**
     * Construct a processor of an gyroscope
     */
    explicit gyroscope_processor(gyroscope& gyroscope);

    /**
     * Fill the gyroscope data
     */
    bool produce(payload_u& payload) const noexcept override;

private:
    /**
     * Gyroscope data isn't currently processed
     * @todo Notch filter
     */
    gyroscope_data_type process(const gyroscope_data_type& raw_data) noexcept override;
};

}