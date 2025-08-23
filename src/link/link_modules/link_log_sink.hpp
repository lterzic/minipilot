#pragma once

#include "link/tx.hpp"
#include "logging/log_sink.hpp"
#include "pb/link/logging/log.nanopb.h"

namespace mp {

class link_log_sink : public log_sink {
public:
    explicit link_log_sink(tx& tx) noexcept;

    void write(const log_s& log) noexcept override;

private:
    tx& m_tx;
};

}