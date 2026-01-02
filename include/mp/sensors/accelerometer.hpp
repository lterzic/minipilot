#pragma once

#include "common/math.hpp"
#include "sensor.hpp"
#include "sensor_processor.hpp"
#include "telemetry/producer.hpp"
#include <emblib/units/speed.hpp>

namespace mp {

/**
 * 3D vector in units of m/s^2
 */
using accelerometer_data_type = vector<emblib::units::meters_per_second_squared<float>, 3>;

/**
 * Accelerometer is a sensor which produces readings of the body
 * acceleration in the local FRD frame, with addition of opposite
 * of the gravity acceleration vector.
 */
using accelerometer = sensor<accelerometer_data_type>;

/**
 * Accelerometer data processor
 */
class accelerometer_processor :
    public sensor_processor<accelerometer>,
    public telemetry_producer<telemetry_channel_e::ACCELEROMETER> {
public:
    /**
     * Construct a processor of an accelerometer
     */
    explicit accelerometer_processor(accelerometer& accelerometer);

    /**
     * Fill the accelerometer data
     */
    bool produce(payload_u& payload) const noexcept override;

private:
    /**
     * Accelerometer processing only does bias removal currently
     */
    accelerometer_data_type process(const accelerometer_data_type& raw_data) noexcept override;

private:
    accelerometer_data_type m_bias;
};

}