#pragma once

#include "link/link.hpp"
#include "telemetry/telemetry.hpp"

namespace mp {

class link_telemetry : private rx_handler {
public:
    explicit link_telemetry(link& link, telemetry& telemetry);

private:
    void handle(payload_u& payload) noexcept override;
    
private:
    telemetry& m_telemtry;
};

}