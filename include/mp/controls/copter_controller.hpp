#pragma once

#include "controller.hpp"
#include "copter/copter.hpp"

namespace mp {

/**
 * Interface for controlling a `copter`
 */
class copter_controller : public controller {
public:
    explicit copter_controller(copter& copter);

    /**
     * Copter controller can switch between any copter control
     * mode at any time, but can't use other modes.
     */
    bool request_transition(const controls_s& controls, const state_s& state) noexcept override;

    /**
     * Run an iteration of the control algorithm and send the
     * output to the copter actuators.
     */
    void update(float dt, const controls_s& controls, const state_s& state) noexcept override;

};

}