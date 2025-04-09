#pragma once

#include "util/math.hpp"
#include "pb/types.pb.h"

namespace mp {

inline void pb_vector3f_set(mp_pb_Vector3f& pb_vec, const vector3f& mp_vec)
{
    pb_vec.x = mp_vec(0);
    pb_vec.y = mp_vec(1);
    pb_vec.z = mp_vec(2);
}

inline void pb_vector4f_set(mp_pb_Vector4f& pb_vec, const vector4f& mp_vec)
{
    pb_vec.w = mp_vec(0);
    pb_vec.x = mp_vec(1);
    pb_vec.y = mp_vec(2);
    pb_vec.z = mp_vec(3);
}
    
}