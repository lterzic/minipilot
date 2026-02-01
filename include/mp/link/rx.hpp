#pragma once

#include "common/chrono.hpp"
#include "handler.hpp"
#include <emblib/io/istream.hpp>
#include <emblib/rtos/task.hpp>
#include <etl/delegate.h>
#include <etl/unordered_map.h>

namespace mp {

/**
 * Receiver thread
 * 
 * Parses received messages and calls the appropriate handler
 */
class rx : public emblib::rtos::static_task<1024> {
    /**
     * RX handler template parameter is not used when calling methods,
     * so we can store all handlers with the same parameter value
     */
    using rx_handler_default = rx_handler<rx_payload_e::TELEMETRY>;

public:
    /**
     * Timeout information to handle failed connections
     */
    struct timeout_s {
        // Time to wait for the next message before calling the callback
        milliseconds_t timeout;
        // Callback to signal there was not message
        etl::delegate<void ()> cb;
    };

public:
    explicit rx(emblib::io::istream& rx_dev, timeout_s timeout) noexcept;

    /**
     * Set a handler for a payload type
     */
    template <rx_payload_e PAYLOAD>
    bool set_handler(rx_handler<PAYLOAD>& handler) noexcept
    {
        return set_handler(PAYLOAD, reinterpret_cast<rx_handler_default&>(handler));
    }

private:
    /**
     * RX handler template parameter is not used at runtime, so
     * all handlers can be cast to default for storage
     */
    bool set_handler(rx_payload_e payload, rx_handler_default& handler) noexcept;

    /**
     * Call handlers on messages as they are received and parsed
     */
    void run() noexcept override;

private:
    // Size of this map should be equal to number of payload types
    etl::unordered_map<rx_payload_e, rx_handler_default*, pb::link::uplink_s::PAYLOAD_COUNT> m_handlers;
    // Serial data receive device
    emblib::io::istream& m_rx_dev;
    // Timeout information
    timeout_s m_timeout;
};

}