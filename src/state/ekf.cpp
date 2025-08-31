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
ekf::ekf(
    const sensor_manager& sensor_manager,
    const model& model
) noexcept :
    state_estimator_periodic(PERIOD, m_stack),
    m_sensor_manager(sensor_manager),
    m_model(model),
    m_kalman({0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    m_sample_count_accelerometer(0),
    m_sample_count_gyroscope(0)
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
    vector3f a_next = m_model.get_linear_acceleration(v, q);

    // Quaternion is updated according to the approximation of the first derivative of
    // the quaternion (w.r.t. time) as a function of angular velocity in the local frame
    const float w1 = w(0);
    const float w2 = w(1);
    const float w3 = w(2);
    const matrixf<4> b {
        {0, -w1, -w2, -w3},
        {w1, 0, w3, -w2},
        {w2, -w3, 0, w1},
        {w3, w2, -w1, 0}
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
    auto jacobian = m_model.get_jacobian(v, w, qv);

    // v_next = v + dt * a
    result(0, 0) = result(1, 1) = result(2, 2) = 1; // dv_dv
    result(0, 3) = result(1, 4) = result (2, 5) = dt; // dv_da

    // a_next = f(v, q)
    result.set_submatrix(3, 0, jacobian.da_dv);
    result.set_submatrix(3, 6, jacobian.da_dq);
    
    const float wx = w(0), wy = w(1), wz = w(2);
    const float qw = qv(0), qx = qv(1), qy = qv(2), qz = qv(3);
    
    // q_next = q + (dt/2) b(w)*q
    const matrixf<4> dq_dq {
        {1, -dt/2*wx, -dt/2*wy, -dt/2*wz},
        {dt/2*wx, 1, dt/2*wz, -dt/2*wy},
        {dt/2*wy, -dt/2*wz, 1, dt/2*wx},
        {dt/2*wz, dt/2*wy, -dt/2*wx, 1}
    };
    const matrixf<4, 3> dq_dw = matrixf<4, 3> {
        {-qx, -qy, -qz},
        {qw, -qz, qy},
        {qz, qw, -qx},
        {-qy, qx, qw}
    } * (dt/2);
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
        {2.f*(ax*qw + ay*qz - qy*(az + G)), 2.f*(ax*qx + ay*qy + qz*(az + G)), 2.f*(-ax*qy + ay*qx - qw*(az + G)), 2*(-ax*qz + ay*qw + qx*(az + G))},
        {2.f*(-ax*qz + ay*qw + qx*(az + G)), 2.f*(ax*qy - ay*qx + qw*(az + G)), 2.f*(ax*qx + ay*qy + qz*(az + G)), 2*(-ax*qw - ay*qz + qy*(az + G))},
        {2.f*(ax*qy - ay*qx + qw*(az + G)), 2.f*(ax*qz - ay*qw - qx*(az + G)), 2.f*(ax*qw + ay*qz - qy*(az + G)), 2*(ax*qx + ay*qy + qz*(az + G))}
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

void ekf::iteration(float dt) noexcept
{
    // TODO: Get Q from the vehicle
    constexpr float v_noise = 1;
    constexpr float a_noise = 5e-1;
    constexpr float q_noise = 1e-1;
    constexpr float w_noise = 5e-1;
    constexpr float wd_noise = 1e-2;
    const auto Q = vectorf<KALMAN_DIM>({
        v_noise, v_noise, v_noise,
        a_noise, a_noise, a_noise,
        q_noise, q_noise, q_noise, q_noise,
        w_noise, w_noise, w_noise,
        wd_noise, wd_noise, wd_noise
    }).as_diagonal();

    // Run the prediction step based on the current state
    auto transition = [this, dt](const auto& state) { return state_transition(state, dt); };
    auto jacobian = [this, dt](const auto& state) { return state_transition_jacob(state, dt); };
    m_kalman.predict(transition, jacobian, Q);

    // Update based on IMU data
    auto accelerometer = m_sensor_manager.get_sensor_reader<sensor_type_e::ACCELEROMETER>();
    auto gyroscope = m_sensor_manager.get_sensor_reader<sensor_type_e::GYROSCOPE>();

    if (accelerometer) {
        size_t current_count = accelerometer->get_sample_count();

        // If there is new accelerometer data available
        if (current_count > m_sample_count_accelerometer) {
            m_sample_count_accelerometer = current_count;
            float acc_nd = accelerometer->get_sensor().get_noise_density();

            const vector3f acc = accelerometer->get_processed().cast<float>();
            const matrix3f cov = matrix3f::diagonal(acc_nd * acc_nd / dt);
            m_kalman.update<3>(
                etl::make_delegate<map_state_to_accelerometer>(),
                etl::make_delegate<map_state_to_accelerometer_jacobian>(),
                cov,
                acc
            );
        }
    }

    if (gyroscope) {
        size_t current_count = gyroscope->get_sample_count();

        // If there is new gyroscope data available
        if (current_count > m_sample_count_gyroscope) {
            m_sample_count_gyroscope = current_count;
            float gyro_nd = gyroscope->get_sensor().get_noise_density();

            const vector3f ang = gyroscope->get_processed().cast<float>();
            const matrix3f cov = matrix3f::diagonal(gyro_nd * gyro_nd / dt);
            m_kalman.update<3>(
                etl::make_delegate<map_state_to_gyroscope>(),
                etl::make_delegate<map_state_to_gyroscope_jacobian>(),
                cov,
                ang
            );
        }
    }

    // Position is integration of velocity and acceleration
    const auto v = get_linear_velocity(m_kalman.get_state());
    const auto a = get_linear_acceleration(m_kalman.get_state());
    m_position += v * dt + a * (dt * dt / 2.f);
}

state_s ekf::create_state() const noexcept
{
    return {
        .position = m_position,
        .velocity = get_linear_velocity(m_kalman.get_state()),
        .acceleration = get_linear_acceleration(m_kalman.get_state()),
        .angular_velocity = get_angular_velocity(m_kalman.get_state()),
        .rotationq = get_rotation_q(m_kalman.get_state()),
        .gyroscope_drift = get_gyro_drift(m_kalman.get_state())
    };
}

}