#pragma once

#include <emblib/units/time.hpp>

namespace mp {

/**
 * Type definition for unit of number of events (samples) per second
 */
using sensor_rate = emblib::units::hertz<size_t>;

/**
 * Sensor interface is the abstraction of the underlying
 * device which provides numeric data at a specific rate.
 */
template <typename data_type>
class sensor {
public:
    /**
     * Public data_type accessor
     */
    using data_t = data_type;

public:
    /**
     * Default constructor and destructor
    */
    explicit sensor() = default;
    virtual ~sensor() = default;

    /**
     * Check if the sensor identifies correctly
     * @note For external devices this could be a read of the ID (WHO_AM_I)
     * register of the device and comparing it to the expected value
     * @return `true` if the sensor responds successfully
     */
    virtual bool ping() noexcept = 0;

    /**
     * Get the current data output rate
     */
    virtual sensor_rate get_rate() const noexcept = 0;  

    /**
     * Read data
     */
    virtual bool read(data_type& out) noexcept = 0;

    /**
     * Noise spectral density
     * @note Return units are assumed to be [data_type]/sqrt(Hz)
     */
    virtual float get_noise_density() const noexcept = 0;
};

}
