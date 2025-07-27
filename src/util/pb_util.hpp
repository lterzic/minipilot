#pragma once

#include "util/math.hpp"
#include "pb/mp/common/types.nanopb.h"

namespace mp {

#define PB_SET(struct, field, value) pb_set(struct.field, struct.has_ ## field, value)

inline void pb_set(pb_mp_Vector3f& pb_vec, bool& pb_has_field, const vector3f& mp_vec)
{
    pb_vec.x = mp_vec(0);
    pb_vec.y = mp_vec(1);
    pb_vec.z = mp_vec(2);
    pb_has_field = true;
}

inline void pb_set(pb_mp_Vector4f& pb_vec, bool& pb_has_field, const vector4f& mp_vec)
{
    pb_vec.w = mp_vec(0);
    pb_vec.x = mp_vec(1);
    pb_vec.y = mp_vec(2);
    pb_vec.z = mp_vec(3);
    pb_has_field = true;
}
    
}