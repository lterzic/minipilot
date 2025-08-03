#pragma once

#include "copter.hpp"
#include "copter_control.hpp"
#include "state/state_estimator.hpp"
#include <emblib/dsp/pid.hpp>
#include <emblib/rtos/mutex.hpp>
#include <emblib/rtos/task.hpp>

namespace mp {

class copter_control_pid : public copter_control, public emblib::task_static<1024> {
public:
    explicit copter_control_pid(
        const copter_params_s& copter_params,
        const state_estimator& state_estimator
    );

    bool set_angular_velocity(vector3f velocity, float thrust) noexcept override;
    bool set_linear_velocity(vector3f velocity, float dir) noexcept override;

    actuation_s get_actuation() const noexcept override
    {
        emblib::scoped_lock lock(m_mutex);
        return {
            .thrust = m_output_thrust,
            .torque = m_output_torque
        };
    }

private:
    void run() noexcept override;

private:
    enum class control_mode_e {
        ANGULAR,
        LINEAR
    };

private:
    const copter_params_s& m_copter_params;
    const state_estimator& m_state_estimator;

    control_mode_e m_control_mode;
    vector3f m_target_w;
    vector3f m_target_v;
    float m_target_dir;

    vector3f m_output_torque;
    float m_output_thrust;

    emblib::pid<vector3f, float> m_angular_velocity_pid;
    emblib::pid<vector3f, float> m_linear_acceleration_pid;

    mutable emblib::mutex m_mutex;
};

}