#pragma once

#include "controller.hpp"
#include "copter/copter.hpp"

namespace mp {

class copter_controller : public controller {
public:
    copter_controller(copter& copter);

    /**
     * Run an iteration of the control algorithm and apply the
     * received actuation on to the copter.
     */
    void update(
        const controls_s& controls,
        const state_s& state,
        float dt) noexcept override;

private:
    /**
     * Run an iteration of the control algorithm and return
     * the actuation to achieve the target control.
     */
    virtual copter_actuation_s update_copter(
        const controls_s& controls,
        const state_s& state,
        float dt) noexcept = 0;

private:
    copter& m_copter;
};

}