#include "link/link.hpp"
#include "logging/logging.hpp"

namespace mp {

link::link(
    emblib::io::istream& rx_dev,
    emblib::io::ostream& tx_dev
) noexcept :
    m_rx_dev(rx_dev),
    m_tx_dev(tx_dev),
    m_handshake(
        rx_dev,
        tx_dev,
        handshake::handshake_cb::create<link, &link::on_handshake>(*this)
    )
{
    // TODO: Create the handshake task here and register the on_handshake callback
}

void link::on_handshake() noexcept
{
    log_info("Handshake established");
    
    m_rx.emplace(m_rx_dev);
    m_tx.emplace(m_tx_dev);

    // TODO: Create and setup all modules which use the link
}

}