#pragma once

#include "common/math.hpp"
#include "common/chrono.hpp"
#include <pblink/common/math.nanopb.h>
#include <pblink/common/chrono.nanopb.h>

namespace mp {

#define PB_SET_FIELD(message, field, value)\
    message.has_ ## field = true;\
    message.field = pb_from(value)

inline pb::vector3f_s pb_from(const vector3f& v) noexcept
{
    return { v(0), v(1), v(2) };
}

inline vector3f pb_to(const pb::vector3f_s& v) noexcept
{
    return { v.x, v.y, v.z };
}

inline pb::quaternionf_s pb_from(const quaternionf& q) noexcept
{
    auto qv = q.as_vector();
    return { qv(0), qv(1), qv(2), qv(3) };
}

inline quaternionf pb_to(const pb::quaternionf_s& q) noexcept
{
    return { q.w, q.x, q.y, q.z };
}

inline pb::time_s pb_from(const milliseconds_t& t) noexcept
{
    return pb::time_s {
        .which_units = pb::time_s::units_e::MS,
        .units = t.value()
    };
}

bool pb_encode_string(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg);

inline pb_callback_s pb_from(const char* str) noexcept
{
    pb_callback_s cb;
    cb.funcs.encode = &pb_encode_string;
    cb.arg = const_cast<char*>(str);
    return cb;
}

template <size_t N>
inline pb_callback_s pb_from(const char* (&str)[N]) noexcept
{
    pb_callback_s cb;
    cb.funcs.encode = &pb_encode_string;
    cb.arg = const_cast<char*>(str);
    return cb;
}

}