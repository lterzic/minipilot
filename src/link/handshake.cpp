#include "link/handshake.hpp"
#include "common/config.hpp"

namespace mp {

handshake::handshake(
    emblib::io::istream& rx_dev,
    emblib::io::ostream& tx_dev,
    handshake_cb cb
) noexcept :
    static_task("handshake", HANDSHAKE_PRIORITY),
    m_rx_dev(rx_dev),
    m_tx_dev(tx_dev),
    m_handshake_cb(cb)
{}

void
handshake::run() noexcept
{
    while (true) {
        // Currently no handshake implementation so just start the link
        m_handshake_cb();
        
        // This task is suspended by the link from the callback while there
        // is an active connection. Once the current connection is lost, this
        // task will be resumed by the link.
    }
}

}