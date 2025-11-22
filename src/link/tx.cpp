#include "tx.hpp"
#include "logging/logging.hpp"
#include <pb_encode.h>

namespace mp {

tx::tx(emblib::io::ostream& tx_dev) :
    m_tx_dev(tx_dev)
{}

bool tx::send_downlink(pb::link::downlink_s& msg)
{
    emblib::rtos::scoped_lock lock(m_mutex);

    // If the transmitter is suspended, can't send messages
    if (m_suspend) {
        return false;
    }

    msg.message_id = m_message_id++;
    // TODO: Fill in the rest of the downlink message data
    
    // Encode the message into a byte array
    char out_buffer[256];
    pb_ostream_t pb_ostream = pb_ostream_from_buffer((pb_byte_t*)out_buffer, sizeof(out_buffer));
    
    if (pb_encode(&pb_ostream, MP_PB_LINK_DOWNLINK_FIELDS, &msg)) {
        auto write_cb = [this](ssize_t status) {
            m_write_smphr.signal_from_isr();
        };

        if (!m_tx_dev.write_async(out_buffer, pb_ostream.bytes_written, write_cb)) {
            return false;
        }
        
        // If the async write doesn't finish in the given time, abort the operation
        if (!m_write_smphr.wait(TX_WRITE_TIMEOUT)) {
            m_tx_dev.abort_async_write();
            log_error("TX write timeout!");
            return false;
        }
    } else {
        log_error("Failed to encode downlink message!");
        return false;
    }
    
    return true;
}

}