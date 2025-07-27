#pragma once

#include "pb/mp/link.nanopb.h"
#include <emblib/io/istream.hpp>
#include <emblib/rtos/task.hpp>
#include <etl/delegate.h>
#include <etl/map.h>

namespace mp {

class pb_rx : public emblib::task {
public:
    using handler_t = etl::delegate<void( const pb_mp_UplinkMessage&)>;

public:
    explicit pb_rx(emblib::io::istream<char>& rx_dev) noexcept;

    /**
     * Set the handler for a certain type of received message
     */
    void set_handler(size_t payload_type, handler_t handler) noexcept
    {
        m_handlers[payload_type] = handler;
    }

private:
    void run() noexcept override;

private:
    // Size of this map should be equal to number of payload types
    etl::map<size_t, handler_t, 4> m_handlers;
    // Serial data receive device
    emblib::io::istream<char>& m_rx_dev;
    // Task stack
    emblib::task_stack_t<1024> m_stack;
};

}