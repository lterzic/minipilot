#pragma once

#include "logging/log_sink.hpp"
#include "pb_tx.hpp"
#include "pb/mp/logging/log.nanopb.h"

namespace mp {

class log_sink_pb : public log_sink {
public:
    explicit log_sink_pb(pb_tx& tx) noexcept;

    void write(const log_s& log) noexcept override;

private:
    pb_tx& m_tx;
};

}