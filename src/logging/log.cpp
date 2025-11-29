#include "log.hpp"
#include <pb_encode.h>

namespace mp {

static constexpr size_t LOG_ARG_MAX_COUNT =
    sizeof(pb::logging::log_s::args) / sizeof(pb::logging::log_arg_s);

bool encode_string(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
{
    const char* str = static_cast<char*>(*arg);
    if (!pb_encode_tag_for_field(stream, field))
        return false;

    return pb_encode_string(stream, reinterpret_cast<const pb_byte_t*>(str), strlen(str));
}

bool log_write_arg(log_s& log, int32_t value) noexcept
{
    if (log.args_count >= LOG_ARG_MAX_COUNT-1)
        return false;
    
    auto& arg = log.args[log.args_count++];
    arg.which_value = pb::logging::log_arg_s::value_e::INT32V;
    arg.value.int32v = value;
    return true;
}

bool log_write_arg(log_s& log, uint32_t value) noexcept
{
    if (log.args_count >= LOG_ARG_MAX_COUNT-1)
        return false;
    
    auto& arg = log.args[log.args_count++];
    arg.which_value = pb::logging::log_arg_s::value_e::UINT32V;
    arg.value.uint32v = value;
    return true;
}

bool log_write_arg(log_s& log, float value) noexcept
{
    if (log.args_count >= LOG_ARG_MAX_COUNT-1)
        return false;
    
    auto& arg = log.args[log.args_count++];
    arg.which_value = pb::logging::log_arg_s::value_e::FLOATV;
    arg.value.floatv = value;
    return true;
}

bool log_write_arg(log_s& log, const char* value) noexcept
{
    if (log.args_count >= LOG_ARG_MAX_COUNT-1)
        return false;
    
    auto& arg = log.args[log.args_count++];
    arg.which_value = pb::logging::log_arg_s::value_e::STRINGV;
    arg.value.stringv.arg = const_cast<char*>(value);
    arg.value.stringv.funcs.encode = &encode_string;
    return true;
}

}