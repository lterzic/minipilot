#pragma once

#include "pb/telemetry/channel.nanopb.h"

namespace mp {

class telemetry_producer {
public:
    /**
     * Populate the channel message
     */
    virtual void fill_channel(mp_pb_telemetry_Channel& channel) const noexcept = 0;
};

}