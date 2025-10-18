#pragma once

#include "state/model.hpp"
#include "state/state_estimator_algo.hpp"
#include <emblib/dsp/kalman.hpp>

namespace mp {

/**
 * Extended kalman filter based state estimator
 */
class ekf final : public state_estimator_algo {
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
    explicit ekf(const model& model) noexcept;

    /**
     * Run an iteration of the algorithm
     */
    state_s update(float dt, const sensors_s& sensors) noexcept override;

private:
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
    // Model used for prediction
    const model& m_model;
    // Kalman filter
    emblib::dsp::kalman<KALMAN_DIM> m_kalman;
    // Kept separately as it's not computed as part
    // of the kalman filter vector
    vector3f m_position;
};

}