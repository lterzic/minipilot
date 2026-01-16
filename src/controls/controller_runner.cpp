#include "controls/controller_runner.hpp"
#include "common/config.hpp"

namespace mp {

controller_runner::controller_runner(
    controller& controller,
    const estimator_runner& estimator,
    milliseconds_t period
) :
    static_task("controller", task_priority_e::TASK_PRIORITY_HIGH),
    m_controller(controller),
    m_estimator(estimator),
    m_period(period)
{
    m_controls.controls = {0};
    m_controls.which_controls = controller::control_mode_e::DISARMED;
    m_controller.set_transition_callback(controller::transition_callback::create<
        controller_runner, &controller_runner::transition_callback>(*this));
}

void
controller_runner::handle(payload_u& payload)
{
    m_queue.send_from_isr(payload.control);
}

void
controller_runner::transition_callback(const controller::controls_s& controls)
{
    m_controls = controls;
    // send message to tx
}

void
controller_runner::run() noexcept
{
    float dt = emblib::units::seconds<float>(m_period).value();

    while (true) {
        auto state = m_estimator.get_state();

        control_msg_s msg;
        while (m_queue.receive(msg, milliseconds_t(0))) {
            switch (msg.which_payload) {
            case control_msg_s::payload_e::REQUEST_TRANSITION:
                m_controller.request_transition(msg.payload.request_transition, state);
                break;
            case control_msg_s::payload_e::UPDATE_CONTROLS:
                m_controls = msg.payload.update_controls;
                break;
            }
        }

        m_controller.update(dt, m_controls, state);
        sleep_periodic(m_period);
    }
}

}