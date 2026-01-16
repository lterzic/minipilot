#pragma once

#include "controller.hpp"
#include "link/rx_handler.hpp"
#include "state/estimator_runner.hpp"
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/queue.hpp>

namespace mp {

class controller_runner :
    private emblib::rtos::static_task<2048>,
    public rx_handler<rx_payload_e::CONTROL> {
public:
    controller_runner(
        controller& controller,
        const estimator_runner& estimator,
        milliseconds_t period
    );

    void handle(payload_u& payload) noexcept override;

private:
    void run() noexcept override;

    void transition_callback(const controller::controls_s& controls);

private:
    using control_msg_s = pb::link::control_uplink_s;

    controller& m_controller;
    controller::controls_s m_controls;
    const estimator_runner& m_estimator;
    const milliseconds_t m_period;
    emblib::rtos::queue<control_msg_s, 4> m_queue;
};

}