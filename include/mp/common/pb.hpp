#pragma once

#include "common/math.hpp"
#include "pb/common/types.nanopb.h"
#include <etl/array.h>

#define PB_SET(struct, field, value) pb_set(struct.field, struct.has_ ## field, value)

namespace mp {

inline void pb_set(pb::vector3f_s& pb_vec, bool& pb_has_field, const vector3f& mp_vec)
{
    pb_vec.x = mp_vec(0);
    pb_vec.y = mp_vec(1);
    pb_vec.z = mp_vec(2);
    pb_has_field = true;
}

template <typename unit_type, size_t SIZE>
inline void pb_set(pb::vector3f_s& pb_vec, bool& pb_has_field, const etl::array<unit_type, SIZE>& array)
{
    pb_vec.x = array[0].value();
    pb_vec.y = array[1].value();
    pb_vec.z = array[2].value();
    pb_has_field = true;
}

inline void pb_set(pb::vector4f_s& pb_vec, bool& pb_has_field, const vector4f& mp_vec)
{
    pb_vec.w = mp_vec(0);
    pb_vec.x = mp_vec(1);
    pb_vec.y = mp_vec(2);
    pb_vec.z = mp_vec(3);
    pb_has_field = true;
}

inline vector3f pb_vector3f(const pb::vector3f_s& pb_vec)
{
    return {pb_vec.x, pb_vec.y, pb_vec.z};
}

bool pb_encode_string(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg);

}