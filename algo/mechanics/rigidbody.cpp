#include "rigidbody.hpp"

namespace mp::algo {

void
rigidbody_s::update(float dt, const vector3f& force, const vector3f& torque) noexcept
{
    const vector3f Iw = moment_of_inertia.matmul(kinematics.angular_velocity);
    const vector3f Ia = torque - kinematics.acceleration.cross(Iw);
    const vector3f ang = Ia.matdivl(moment_of_inertia);
    const vector3f acc = force / mass;

    kinematics.update(dt, acc, ang);
}

}