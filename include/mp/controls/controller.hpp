#pragma once

#include "pb/control/control.nanopb.h"
#include "state/state.hpp"
#include <etl/delegate.h>

namespace mp {

class controller {
public:
    /**
     * All available control types used for controlling the vehicle.
     */
    using controls_s = pb::control::controls_s;

    /**
     * For each vehicle type there might be one or more control
     * modes which are used to define target actuation of the vehicle.
     */
    using control_mode_e = controls_s::mode_e;

    /**
     * When a change of control mode happens, this is called to notify
     * the user. Control mode might change as the result of a transition,
     * but can also change by itself if a certain state is reached, such
     * as speed to low, algorithm instability, etc.
     */
    using transition_callback = etl::delegate<void (const controls_s&)>;

public:
    /**
     * Transition to a new control mode, with the specified inital
     * control values.
     * @returns `true` if the transition is started, `false` if
     * conditions for the new mode are not met.
     * @note This only starts the transition. The transition itself
     * might take some time, for example if changing wing configuration.
     */
    virtual bool request_transition(const controls_s& controls, const state_s& state) noexcept = 0;

    /**
     * Run an iteration of the control algorithm. Type of the controls
     * in the argument should match the current controller mode.
     */
    virtual void update(const controls_s& controls, const state_s& state, float dt) noexcept = 0;

protected:
    /**
     * Notify that a transition has finished.
     */
    void notify_transition(const controls_s& new_controls) noexcept;

private:
    transition_callback m_callback;
};

}