#pragma once

namespace mp {

class motor {

public:
    /**
     * Default constructor and destructor
     */
    explicit motor() = default;
    virtual ~motor() = default;

    /**
     * Set the motor throttle
     * @param throttle Value in the range `[0, 1]`
     * @returns true if successful
     * @todo Change to `set_throttle`
     */
    virtual bool write_throttle(float throttle) noexcept = 0;
    
    /**
     * Read the current motor throttle
     * @param throttle Assigned value must be between 0 and 1 (inclusive)
     * @returns true if successful
     */
    virtual bool read_throttle(float& throttle) noexcept = 0;

    /**
     * Assuming a first order approximation of the transfer
     * function for the throttle response in the form:
     * `G(s) = 1 / (1 + s k_t)`
     * returns `k_t`
     */
    virtual float get_time_constant() const noexcept = 0;

    /**
     * Return true if the shaft is spinning CCW when
     * looking at the shaft from the front of the motor
     */
    virtual bool get_direction() const noexcept = 0;

};

}
