#include "telemetry/producer_registry.hpp"

namespace mp {

telemetry_producer_registry&
telemetry_producer_registry::get_instance() noexcept
{
    static telemetry_producer_registry s_registry;
    return s_registry;
}

bool
telemetry_producer_registry::add_producer(telemetry_channel_e channel, telemetry_producer_base& producer) noexcept
{
    if (m_producers[channel].full())
        return false;
    
    m_producers[channel].push_back(&producer);
    return true;
}
    
}