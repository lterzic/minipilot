#pragma once

#include "controller.hpp"
#include "link/rx_handler.hpp"
#include "state/estimator_runner.hpp"
#include <emblib/rtos/task.hpp>

namespace mp {

template <typename controller_type>
class controller_runner :
    public rx_handler<rx_payload_e::CONTROL>,
    private emblib::rtos::static_task<2048> {
public:
    static_assert(std::is_base_of_v<controller, controller_type>);

    /**
     * Create a runner for a controller
     */
    controller_runner(
        controller_type& controller,
        const estimator_runner& estimator,
        milliseconds_t period
    ) :
        m_controller(controller),
        m_controls(pb::control::disarmed_s {}),
        m_estimator(estimator),
        m_period(period)
    {}

    /**
     * Receive control messages and pass them to controller implementation.
     */
    void handle(payload_u& payload) noexcept override
    {
        switch (payload.control.which_payload) {
        control_msg_s::payload_e::REQUEST_TRANSITION:
            break;
        control_msg_s::payload_e::UPDATE_CONTROLS:
            break;
        default:
            break;
        }
    }

private:
    /**
     * Run controller in a loop.
     */
    void run() noexcept override
    {
        while (true) {
            sleep_periodic(m_period);
        }
    }

    /**
     * Notify the user that the control mode has changed, and update
     * the allowed control type.
     */
    void transition_callback(const controller::controls_s& controls)
    {
        m_controls = controls;
        // TODO: Send message over tx
    }

private:
    using control_msg_s = pb::link::control_uplink_s;

    controller_type& m_controller;
    controller::controls_s m_controls;
    const estimator_runner& m_estimator;
    const milliseconds_t m_period;
};

}