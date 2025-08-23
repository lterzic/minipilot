#pragma once

#include "rx.hpp"
#include "tx.hpp"
#include "handshake.hpp"
#include <etl/optional.h>

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
    void on_handshake() noexcept;

private:
    // These devices are used both for rx and tx and handshake
    emblib::io::istream& m_rx_dev;
    emblib::io::ostream& m_tx_dev;

    // RX and TX are created only when the handshake is completed
    // Using optional to avoid using pointers and heap
    etl::optional<rx> m_rx;
    etl::optional<tx> m_tx;

    // Handshake thread
    handshake m_handshake;
};

}