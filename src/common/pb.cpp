#include "pb.hpp"
#include <pb_encode.h>

namespace mp {

bool encode_string(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
{
    const char* str = static_cast<char*>(*arg);
    if (!pb_encode_tag_for_field(stream, field))
        return false;

    return pb_encode_string(stream, reinterpret_cast<const pb_byte_t*>(str), strlen(str));
}

}