#pragma once

#include "link/tx.hpp"
#include "logging/log_sink.hpp"
#include "pb/link/logging/log.nanopb.h"

namespace mp {

class log_sink_link : public log_sink {
public:
    explicit log_sink_link(tx& tx) noexcept;

    void write(const log_s& log) noexcept override;

private:
    tx& m_tx;
};

}