#include "parameter_manager.hpp"
#include "common/chrono.hpp"
#include "logging/logging.hpp"
#include <pb_decode.h>

namespace mp {

parameter_manager::parameter_manager(
    emblib::io::istream &istorage,
    emblib::io::ostream &ostorage
) :
    m_istorage(istorage),
    m_ostorage(ostorage)
{
    if (!load_from_storage()) {
        log_info("No parameters found in storage, loading default");
        load_default();
    } else {
        log_info("Parameters loaded from storage");
    }
}

bool
parameter_manager::load_from_storage()
{
    char buffer[MP_PB_CONFIG_CONFIG_SIZE];
    ssize_t status = m_istorage.read(buffer, sizeof(buffer), milliseconds_t(10));
    
    if (status <= 0) {
        log_error("Failed to read parameters from storage");
        return false;
    }

    pb_istream_t buf_istream = pb_istream_from_buffer((const pb_byte_t*)buffer, status);
    return pb_decode(&buf_istream, MP_PB_CONFIG_CONFIG_FIELDS, &m_parameters);
}

void
parameter_manager::load_default()
{
    m_parameters = {0};
}

}