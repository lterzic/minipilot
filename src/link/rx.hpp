#pragma once

#include "pb/link/link.nanopb.h"
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
class rx : private emblib::rtos::static_task<1024> {
public:
    /**
     * Handler for one type of uplink message payload
     * @note Handlers are executed in the receiver thread context
     */
    using handler_cb = etl::delegate<void (const mp_pb_link_UplinkMessage&)>;

public:
    explicit rx(emblib::io::istream& rx_dev) noexcept;

    /**
     * Set the handler for a certain type of received message
     */
    bool set_handler(pb_size_t payload_type, handler_cb cb) noexcept
    {
        if (m_handlers.find(payload_type) != m_handlers.end())
            return false;
        m_handlers[payload_type] = cb;
        return true;
    }

private:
    void run() noexcept override;

private:
    // Size of this map should be equal to number of payload types
    etl::unordered_map<pb_size_t, handler_cb, 4> m_handlers;
    // Serial data receive device
    emblib::io::istream& m_rx_dev;
};

}