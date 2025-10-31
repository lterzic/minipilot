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

    copter_actuation_s update(const controls_v& input, const state_s& state, float dt) noexcept override;

private:
    copter_actuation_s update_angular(const angular_controls_s& input, const state_s& state, float dt) noexcept;
    angular_controls_s update_linear(const linear_controls_s& input, const state_s& state, float dt) noexcept;

private:
    const copter_params_s& m_copter_params;
    
    emblib::dsp::pid<vector3f, float> m_angular_velocity_pid;
    emblib::dsp::pid<vector3f, float> m_linear_acceleration_pid;
};

}