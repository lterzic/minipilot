#pragma once

#include "link/rx.hpp"
#include "link/tx.hpp"
#include "link/handshake.hpp"

namespace mp {

/**
 * Handles the handshake process and creates a link
 */
class link {
public:
    explicit link(
        emblib::io::istream& rx_dev,
        emblib::io::ostream& tx_dev
    ) noexcept;

private:
    /**
     * When a handshake is established, create the receiver
     * and transmitter
     * @note This method may be called multiple times in situations
     * where a connection is lost and the handshake system is restarted
     * mid session
     * @todo Add handshake (link) parameters as the argument
     */
    void on_handshake();

    /**
     * If the receiver (RX) does not receive any message within
     * some time interval, it will be assumed that the connection
     * is lost. This method is than called to restart the handshake
     * thread to allow new connection to be formed. At that point
     * the old connection is invalidated.
     */
    void on_disconnect();

private:
    // Handshake thread
    handshake m_handshake;
    // Receiver and transmitter
    rx m_rx;
    tx m_tx;

    // Link modules are friends so that rx and tx can be accessed
    // without exposing them with getters or as public
    friend class link_copter;
    friend class link_logging;
    friend class telemetry;
};

}