#pragma once

#include "copter/copter_controller.hpp"
#include <emblib/dsp/pid.hpp>
#include <emblib/rtos/mutex.hpp>

namespace mp {

class copter_controller_pid : public copter_controller {
public:
    explicit copter_controller_pid(
        copter& copter,
        const state_estimator& state_estimator,
        const copter_params_s& copter_params
    );

    bool set_angular_velocity(vector3f velocity, float thrust) noexcept override;
    bool set_linear_velocity(vector3f velocity, float dir) noexcept override;

private:
    actuation_s iterate(const state_s& state, float dt) noexcept override;

private:
    enum class control_mode_e {
        ANGULAR,
        LINEAR
    };

    const copter_params_s& m_copter_params;

    control_mode_e m_control_mode;
    vector3f m_target_w;
    vector3f m_target_v;
    float m_target_dir;
    actuation_s m_output;
    
    emblib::dsp::pid<vector3f, float> m_angular_velocity_pid;
    emblib::dsp::pid<vector3f, float> m_linear_acceleration_pid;

    emblib::rtos::mutex m_mutex;
    emblib::rtos::task_stack<1024> m_stack;
};

}