#pragma once

#include "copter/copter.hpp"
#include "copter/copter_controller.hpp"
#include "state/state_estimator.hpp"
#include <emblib/dsp/pid.hpp>
#include <emblib/rtos/mutex.hpp>

namespace mp {

class copter_controller_pid : public copter_controller {
public:
    explicit copter_controller_pid(
        const state_estimator& state_estimator,
        const copter_params_s& copter_params
    );

    actuation_s iterate(float dt) noexcept override;

private:
    const state_estimator& m_state_estimator;
    const copter_params_s& m_copter_params;

    angular_controls_s m_angular_target;
    
    emblib::dsp::pid<vector3f, float> m_angular_velocity_pid;
    emblib::dsp::pid<vector3f, float> m_linear_acceleration_pid;
};

}