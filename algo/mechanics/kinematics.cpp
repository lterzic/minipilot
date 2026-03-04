#include "kinematics.hpp"
#include "mp/algo/math/matrix.hpp"

namespace mp::algo {

void
kinematics_s::update(float dt, const vector3f& lin_acc, const vector3f& ang_acc) noexcept
{
    position += velocity * dt + acceleration * (dt * dt * 0.5f);
    velocity += acceleration * dt;
    acceleration = lin_acc;

    const float wx = angular_velocity(0);
    const float wy = angular_velocity(1);
    const float wz = angular_velocity(2);
    const matrix<float, 4> b {
        {0, -wx, -wy, -wz},
        {wx, 0, wz, -wy},
        {wy, -wz, 0, wx},
        {wz, wy, -wx, 0}
    };

    auto qv = rotation.as_vector();
    qv += (dt * 0.5f) * b.matmul(qv);
    qv /= qv.norm();
    rotation = quaternionf(qv(0), qv(1), qv(2), qv(3));

    angular_velocity += ang_acc * dt;
}

} // namespace mp::algo
