#pragma once

#include <emblib/io/istream.hpp>
#include <emblib/io/ostream.hpp>
#include <emblib/rtos/task.hpp>
#include <etl/delegate.h>

namespace mp {

/**
 * Handshake thread
 * 
 * Transmits a beacon periodically (define period in mp_config),
 * and responds to requests trying to setup a link. When a request
 * is valid it transmits a valid response to the user (GCS), and calls
 * a callback with link parameters to signal to minipilot that a link is valid.
 * This callback is provided by the `link` class.
 */
class handshake : public emblib::rtos::static_task<512> {
public:
    /**
     * Handshake callback
     * @todo Add link parameters as argument
     */
    using handshake_cb = etl::delegate<void ()>;

public:
    explicit handshake(
        emblib::io::istream& rx_dev,
        emblib::io::ostream& tx_dev,
        handshake_cb cb
    ) noexcept;

private:
    void run() noexcept override;

private:
    emblib::io::istream& m_rx_dev;
    emblib::io::ostream& m_tx_dev;
    handshake_cb m_handshake_cb;
};

}