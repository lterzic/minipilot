#include "link/link.hpp"
#include "logging/logging.hpp"

namespace mp {

link::link(
    emblib::io::istream& rx_dev,
    emblib::io::ostream& tx_dev
) noexcept :
    m_handshake(
        rx_dev,
        tx_dev,
        etl::make_delegate<link, &link::on_handshake>(*this)
    ),
    m_rx(
        rx_dev,
        rx::timeout_s {
            RX_TIMEOUT,
            etl::make_delegate<link, &link::on_disconnect>(*this)
        }),
    m_tx(tx_dev)
{
    // Handshake task is running on start since there is no active connection.
    // Once the connection (link) is established, handshake task suspends.
    m_rx.suspend();
    m_tx.suspend();
}

void link::on_handshake()
{
    log_info("Handshake established");

    // TODO: Get session data from the handshake
    
    m_tx.resume();
    m_rx.resume();
    m_handshake.suspend();
}

void link::on_disconnect()
{
    // TODO: Invalidate the current session
    m_handshake.resume();
    m_tx.suspend();
    m_rx.suspend();
}

}