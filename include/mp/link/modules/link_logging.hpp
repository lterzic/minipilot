#pragma once

#include "link/link.hpp"
#include "logging/logger.hpp"

namespace mp {

class link_logging : private rx_handler<rx_payload_e::LOGGING>, private log_sink {
public:
    explicit link_logging(link& link, logger& logger);

private:
    void write(const log_s& log) noexcept override;
    void handle(payload_u& payload) noexcept override;
    
private:
    tx& m_tx;
};

}