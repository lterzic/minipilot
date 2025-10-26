#include "state/ekf.hpp"
#include "common/constants.hpp"

namespace mp {

// Extract the velocity vector from the kalman state vector
static vector3f get_linear_velocity(const ekf::state_vec_t& state) noexcept
{
    return {state(0), state(1), state(2)};
}

// Extract the acceleration vector from the kalman state vector
static vector3f get_linear_acceleration(const ekf::state_vec_t& state) noexcept
{
    return {state(3), state(4), state(5)};
}

// Extract the rotation quaternion from the kalman state vector
static quaternionf get_rotation_q(const ekf::state_vec_t& state) noexcept
{
    return {state(6), state(7), state(8), state(9)};
}

// Extract the angular velocity vector from the kalman state vector
static vector3f get_angular_velocity(const ekf::state_vec_t& state) noexcept
{
    return {state(10), state(11), state(12)};
}

// Extract the gyro drift vector from the kalman state vector
static vector3f get_gyro_drift(const ekf::state_vec_t& state) noexcept
{
    return {state(13), state(14), state(15)};
}

// Initialize the state and underlying task implementation
ekf::ekf(const model& model) noexcept :
    m_model(model),
    m_kalman({0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    m_position({0, 0, 0})
{}

// For implementation details view docs for this task
ekf::state_vec_t
ekf::state_transition(const state_vec_t& state, float dt) const noexcept
{
    const auto v = get_linear_velocity(state);
    const auto a = get_linear_acceleration(state);
    const auto q = get_rotation_q(state);
    const auto w = get_angular_velocity(state);

    // Acceleration is computed by the vehicle based on current actuator settings and the
    // dynamical model of the vehicle, and the velocity is the integration of acceleration
    vector3f v_next = v + dt * a;
    vector3f a_next = m_model.get_linear_acceleration(v, a, q);

    // Quaternion is updated according to the approximation of the first derivative of
    // the quaternion (w.r.t. time) as a function of angular velocity in the local frame
    const float wx = w(0);
    const float wy = w(1);
    const float wz = w(2);
    const matrixf<4> b {
        {0, -wx, -wy, -wz},
        {wx, 0, wz, -wy},
        {wy, -wz, 0, wx},
        {wz, wy, -wx, 0}
    };
    
    vector4f qv = q.as_vector();
    vector4f qv_next = qv + (dt / 2.f) * b.matmul(qv);
    // Normalize the quaternion due to numerical errors
    qv_next /= qv_next.norm();
    
    // Angular acceleration is the first derivative of angular velocity and
    // is calculated according to the Euler's equations for a rotating reference frame
    vector3f dw = m_model.get_angular_acceleration(v, w, q);
    vector3f w_next = w + dt * dw;

    // We're not expecting the drift to change from iteration to iteration
    const vector3f wd = get_gyro_drift(state);

    return {
        v_next(0), v_next(1), v_next(2),
        a_next(0), a_next(1), a_next(2),
        qv_next(0), qv_next(1), qv_next(2), qv_next(3),
        w_next(0), w_next(1), w_next(2),
        wd(0), wd(1), wd(2)
    };
}

matrixf<ekf::KALMAN_DIM>
ekf::state_transition_jacob(const state_vec_t& state, float dt) const noexcept
{
    matrixf<KALMAN_DIM> result {0};

    const auto v = get_linear_velocity(state);
    const auto a = get_linear_acceleration(state);
    const auto q = get_rotation_q(state);
    const auto w = get_angular_velocity(state);
    const auto qv = q.as_vector();

    // Not const because some matrices are multiplied by dt before
    // being inserted into the result matrix
    auto jacobian = m_model.get_jacobian(v, a, qv, w);

    // v_next = v + dt * a
    result(0, 0) = result(1, 1) = result(2, 2) = 1; // dv_dv
    result(0, 3) = result(1, 4) = result (2, 5) = dt; // dv_da

    // a_next = f(v, a, q)
    result.set_submatrix(3, 0, jacobian.da_dv);
    result.set_submatrix(3, 3, jacobian.da_da);
    result.set_submatrix(3, 6, jacobian.da_dq);
    
    const float wx = w(0), wy = w(1), wz = w(2);
    const float qw = qv(0), qx = qv(1), qy = qv(2), qz = qv(3);
    const float dt2 = dt / 2.f;
    
    // q_next = q + (dt/2) b(w)*q
    const matrixf<4> dq_dq {
        {1, -dt2*wx, -dt2*wy, -dt2*wz},
        {dt2*wx, 1, dt2*wz, -dt2*wy},
        {dt2*wy, -dt2*wz, 1, dt2*wx},
        {dt2*wz, dt2*wy, -dt2*wx, 1}
    };
    const matrixf<4, 3> dq_dw = matrixf<4, 3> {
        {-qx, -qy, -qz},
        {qw, -qz, qy},
        {qz, qw, -qx},
        {-qy, qx, qw}
    } * dt2;
    result.set_submatrix(6, 6, dq_dq);
    result.set_submatrix(6, 10, dq_dw);

    // w_next = w + dt * dw(v, q, w)
    jacobian.ddw_dv *= dt;
    jacobian.ddw_dq *= dt;
    jacobian.ddw_dw *= dt;
    result.set_submatrix(10, 0, jacobian.ddw_dv);
    result.set_submatrix(10, 6, jacobian.ddw_dq);
    result.set_submatrix(10, 10, jacobian.ddw_dw);
    
    // dw_dw
    result(10, 10) += 1.f;
    result(11, 11) += 1.f;
    result(12, 12) += 1.f;

    // dwd_dwd
    result(13, 13) = 1.f;
    result(14, 14) = 1.f;
    result(15, 15) = 1.f;

    return result;
}

static vectorf<3> map_state_to_accelerometer(const ekf::state_vec_t& state)
{
    const auto a = get_linear_acceleration(state);
    const auto q = get_rotation_q(state);

    // Accelerometer readings are acceleration - gravity vector
    // in the local reference frame (+ noise)
    return q.conjugate().rotate_vec(a - GV);
}

static matrixf<3, ekf::KALMAN_DIM> map_state_to_accelerometer_jacobian(const ekf::state_vec_t& state)
{
    const auto a = get_linear_acceleration(state);
    const auto qv = get_rotation_q(state).as_vector();

    const float ax = a(0), ay = a(1), az = a(2);
    const float qw = qv(0), qx = qv(1), qy = qv(2), qz = qv(3);
    
    // d(a_exp)/d(a)
    const matrixf<3> da_da {
        {qw*qw + qx*qx - qy*qy - qz*qz, 2.f*(qw*qz + qx*qy), 2.f*(-qw*qy + qx*qz)},
        {2.f*(-qw*qz + qx*qy), qw*qw - qx*qx + qy*qy - qz*qz, 2.f*(qw*qx + qy*qz)},
        {2.f*(qw*qy + qx*qz), 2.f*(-qw*qx + qy*qz), qw*qw - qx*qx - qy*qy + qz*qz}
    };

    // d(a_exp)/d(qv)
    const matrixf<3, 4> da_dq {
        {2.f*(ax*qw + ay*qz - qy*(az - G)), 2.f*(ax*qx + ay*qy + qz*(az - G)), 2.f*(-ax*qy + ay*qx - qw*(az - G)), 2.f*(-ax*qz + ay*qw + qx*(az - G))},
        {2.f*(-ax*qz + ay*qw + qx*(az - G)), 2.f*(ax*qy - ay*qx + qw*(az - G)), 2.f*(ax*qx + ay*qy + qz*(az - G)), 2.f*(-ax*qw - ay*qz + qy*(az - G))},
        {2.f*(ax*qy - ay*qx + qw*(az - G)), 2.f*(ax*qz - ay*qw - qx*(az - G)), 2.f*(ax*qw + ay*qz - qy*(az - G)), 2.f*(ax*qx + ay*qy + qz*(az - G))}
    };

    matrixf<3, ekf::KALMAN_DIM> jacobian(0);
    jacobian.set_submatrix(0, 3, da_da);
    jacobian.set_submatrix(0, 6, da_dq);
    return jacobian;
}

static vectorf<3> map_state_to_gyroscope(const ekf::state_vec_t& state)
{
    const auto w = get_angular_velocity(state);
    const auto wd = get_gyro_drift(state);

    return w + wd;
}

static matrixf<3, ekf::KALMAN_DIM> map_state_to_gyroscope_jacobian(const ekf::state_vec_t& state)
{
    matrixf<3, ekf::KALMAN_DIM> jacobian(0);
    
    // d(w_exp)/d(w)
    jacobian(0, 10) = jacobian(1, 11) = jacobian(2, 12) = 1.f;
    // d(w_exp)/d(wd)
    jacobian(0, 13) = jacobian(1, 14) = jacobian(2, 15) = 1.f;

    return jacobian;
}

state_s
ekf::update(float dt, const sensors_s& sensors) noexcept
{
    // Run the prediction step based on the current state
    auto transition = [this, dt](const auto& state) { return state_transition(state, dt); };
    auto jacobian = [this, dt](const auto& state) { return state_transition_jacob(state, dt); };
    m_kalman.predict(transition, jacobian, m_model.get_process_noise());

    if (sensors.accelerometer) {
        float acc_nd = sensors.accelerometer->second;
        const vector3f& acc = sensors.accelerometer->first;
        const matrix3f cov = matrix3f::diagonal(acc_nd * acc_nd / dt);
        m_kalman.update<3>(
            etl::make_delegate<map_state_to_accelerometer>(),
            etl::make_delegate<map_state_to_accelerometer_jacobian>(),
            cov,
            acc
        );
    }

    if (sensors.gyroscope) {
        float gyro_nd = sensors.gyroscope->second;
        const vector3f& ang = sensors.gyroscope->first;
        const matrix3f cov = matrix3f::diagonal(gyro_nd * gyro_nd / dt);
        m_kalman.update<3>(
            etl::make_delegate<map_state_to_gyroscope>(),
            etl::make_delegate<map_state_to_gyroscope_jacobian>(),
            cov,
            ang
        );
    }

    // Position is integration of velocity and acceleration
    const auto v = get_linear_velocity(m_kalman.get_state());
    const auto a = get_linear_acceleration(m_kalman.get_state());
    m_position += v * dt + a * (dt * dt / 2.f);

    return {
        .position = m_position,
        .velocity = v,
        .acceleration = a,
        .angular_velocity = get_angular_velocity(m_kalman.get_state()),
        .rotationq = get_rotation_q(m_kalman.get_state()),
        .gyroscope_drift = get_gyro_drift(m_kalman.get_state())
    };
}

}