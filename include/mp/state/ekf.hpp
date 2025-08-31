#pragma once

#include "sensors/sensor_manager.hpp"
#include "state/model.hpp"
#include "state/state_estimator_periodic.hpp"
#include <emblib/dsp/kalman.hpp>

namespace mp {

/**
 * Extended kalman filter based state estimator
 */
class ekf : public state_estimator_periodic {
    /**
     * Stack size experimentally determined
     */
    static constexpr size_t STACK_SIZE = 25000;

    /**
     * Running at 50Hz by default
     */
    static constexpr auto PERIOD = milliseconds_t(20);

public:
    /**
     * Dimension of the state vector used by the kalman filter
     * 3 - velocity
     * 3 - acceleration
     * 4 - rotation quaternion
     * 3 - angular velocity
     * 3 - gyro drift
     * @note Public since used by ekf.cpp static functions
     */
    static constexpr size_t KALMAN_DIM = 16;

    /**
     * Convenience typedef
     * @note Public since used by ekf.cpp static functions
     */
    using state_vec_t = vectorf<KALMAN_DIM>;

public:
    explicit ekf(
        const sensor_manager& sensor_manager,
        const model& model
    ) noexcept;

private:
    /**
     * Run an iteration of the algorithm
     */
    void iteration(float dt) noexcept override;
    
    /**
     * Create state from an internal representation
     */
    state_s create_state() const noexcept override;
    
    /**
     * Kalman filter state transition - `f`
     * @note View docs for this task for reasoning
     */
    state_vec_t state_transition(const state_vec_t& state, float dt) const noexcept;

    /**
     * Kalman filter state transition jacobian - `F`
     * 
     * Represents the derivative of `state_transition` function with respect to the state vector
     */
    matrixf<KALMAN_DIM> state_transition_jacob(const state_vec_t& state, float dt) const noexcept;

private:
    // Sensor manager
    const sensor_manager& m_sensor_manager;
    // Model used for prediction
    const model& m_model;
    // Sample counts for each sensor to determine
    // if new data is available
    size_t m_sample_count_accelerometer;
    size_t m_sample_count_gyroscope;

    // Kalman filter
    emblib::dsp::kalman<KALMAN_DIM> m_kalman;
    // Kept separately as it's not computed as part
    // of the kalman filter vector
    vector3f m_position;

    emblib::rtos::task_stack<STACK_SIZE> m_stack;
};

}