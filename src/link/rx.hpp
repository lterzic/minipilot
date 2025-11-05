#pragma once

#include "common/chrono.hpp"
#include "pb/link/link.nanopb.h"
#include <emblib/io/istream.hpp>
#include <emblib/rtos/task.hpp>
#include <etl/delegate.h>
#include <etl/unordered_map.h>

namespace mp {

/**
 * Handler for one type of uplink message payload
 * @note Handlers are executed in the receiver thread context
 */
struct rx_handler {
    /**
     * Payload union
     */
    using payload_u = mp_pb_link_Uplink::_mp_pb_link_Uplink_payload;

    /**
     * Process the received messages
     * @note It is guaranteed that the payload matches the
     * register payload tag when calling `rx::set_handler`
     */
    virtual void handle(payload_u& payload) noexcept = 0;
};

/**
 * Receiver thread
 * 
 * Parses received messages and calls the appropriate handler
 */
class rx : public emblib::rtos::static_task<1024> {
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
     * Set the handler for a certain type of received message
     */
    bool set_handler(pb_size_t payload_type, rx_handler& handler) noexcept;

private:
    void run() noexcept override;

private:
    // Size of this map should be equal to number of payload types
    etl::unordered_map<pb_size_t, rx_handler*, 4> m_handlers;
    // Serial data receive device
    emblib::io::istream& m_rx_dev;
    // Timeout information
    timeout_s m_timeout;
};

}