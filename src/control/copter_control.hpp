#pragma once

#include "common/chrono.hpp"
#include "common/config.hpp"
#include "common/math.hpp"
#include "state/state_estimator.hpp"
#include "vehicles/copter.hpp"
#include <emblib/rtos/task.hpp>

namespace mp {

/**
 * Interface for controlling a `copter`
 */
class copter_control : private emblib::rtos::task {
public:
    template <size_t STACK_SIZE>
    explicit copter_control(
        copter& copter,
        const state_estimator& state_estimator,
        milliseconds_t period,
        emblib::rtos::task_stack<STACK_SIZE>& stack
    ) :
        task("Copter control", COPTER_CONTROL_PRIORITY, stack),
        m_copter(copter),
        m_state_estimator(state_estimator),
        m_period(period)
    {}

    /**
     * Set the desired angular velocity vector and thrust
     * @param velocity Angular velocity vector in forward-right-down system in radians per second
     * @param thrust Thrust in the "UP" direction in Newtons
     * @returns `false` if the given angular velocity or thrust can't be achieved
     */
    virtual bool set_angular_velocity(vector3f velocity, float thrust) noexcept = 0;

    /**
     * Set the desired velocity vector and direction
     * @param velocity Velocity vector in the global NED coordinate system in meters per second
     * @param dir Direction in radians with 0 being North, going clockwise
     * @returns `false` if the given velocity can't be achieved
     */
    virtual bool set_linear_velocity(vector3f velocity, float dir) noexcept = 0;

protected:
    struct actuation_s {
        // Thrust in Newtons in the "UP" direction
        float thrust;
        // Torque in the forward-right-down system in Newton-meters
        vector3f torque;
    };

private:
    /**
     * Get the required actuation to achieve the last set command
     */
    virtual actuation_s iterate(const state_s& state, float dt) noexcept = 0;

    /**
     * Run an iteration of the control algorithm and assign
     * actuation values to the copter vehicle
     */
    void run() noexcept override;

private:
    copter& m_copter;
    const state_estimator& m_state_estimator;
    
    milliseconds_t m_period;
};

}