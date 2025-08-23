#include "handshake.hpp"
#include "mp_config.hpp"

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
    // Currently no handshake implementation so just start the link
    m_handshake_cb();
    
    // TODO: Suspend here (currently exiting since no suspend implementation yet)
}

}