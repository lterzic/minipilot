#include "parameter_manager.hpp"

namespace mp {

parameter_manager::parameter_manager(
    emblib::io::istream &istorage,
    emblib::io::ostream &ostorage
) :
    m_istorage(istorage),
    m_ostorage(ostorage)
{
    if (!load_from_storage())
        load_default();
}

bool
parameter_manager::load_from_storage()
{
    // Read from istorage and try to decode
    return false;
}

void
parameter_manager::load_default()
{
    // Fill with default values
}

}