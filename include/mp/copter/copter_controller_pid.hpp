#pragma once

#include "copter/copter.hpp"
#include "copter/copter_controller.hpp"
#include <emblib/dsp/pid.hpp>

namespace mp {

class copter_controller_pid : public copter_controller {
public:
    explicit copter_controller_pid(
        const copter_params_s& copter_params
    );

    actuation_s update(float dt, const control_v& controls, const state_s& state) noexcept override;

private:
    const copter_params_s& m_copter_params;
    angular_controls_s m_angular_target;
    
    emblib::dsp::pid<vector3f, float> m_angular_velocity_pid;
    emblib::dsp::pid<vector3f, float> m_linear_acceleration_pid;
};

}