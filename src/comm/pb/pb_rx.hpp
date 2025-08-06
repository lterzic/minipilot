#pragma once

#include "pb/mp/link.nanopb.h"
#include <emblib/io/istream.hpp>
#include <emblib/rtos/task.hpp>
#include <etl/unordered_map.h>

namespace mp {

struct rx_handler {
    /**
     * Message payload should always be the same as the
     * payload this handler is registered for
     */
    virtual void handle(const pb_mp_UplinkMessage& message) noexcept = 0;
};

class pb_rx : private emblib::task_static<1024> {
public:
    explicit pb_rx(emblib::io::istream<char>& rx_dev) noexcept;

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
    emblib::io::istream<char>& m_rx_dev;
};

}