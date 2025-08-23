#pragma once

#include "pb/link/link.nanopb.h"
#include <emblib/io/istream.hpp>
#include <emblib/rtos/task.hpp>
#include <etl/unordered_map.h>

namespace mp {

/**
 * Handler for one type of uplink message payload
 * @note Handlers are executed in the receiver thread context
 */
struct rx_handler {
    /**
     * Message payload should always be the same as the
     * payload this handler is registered for
     */
    virtual void handle(const mp_pb_link_UplinkMessage& message) noexcept = 0;
};

/**
 * Receiver thread
 * 
 * Parses received messages and calls the appropriate handler
 */
class rx : private emblib::rtos::static_task<1024> {
public:
    explicit rx(emblib::io::istream& rx_dev) noexcept;

    /**
     * Set the handler for a certain type of received message
     */
    void set_handler(pb_size_t payload_type, rx_handler* handler) noexcept
    {
        m_handlers[payload_type] = handler;
    }

private:
    void run() noexcept override;

private:
    // Size of this map should be equal to number of payload types
    etl::unordered_map<pb_size_t, rx_handler*, 4> m_handlers;
    // Serial data receive device
    emblib::io::istream& m_rx_dev;
};

}