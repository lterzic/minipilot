#include "pb_tx.hpp"
#include "logging/log.hpp"
#include <pb_encode.h>

namespace mp {

pb_tx::pb_tx(emblib::io::ostream<char>& tx_dev) :
    m_tx_dev(tx_dev)
{}

bool pb_tx::send_downlink(payload_cb_t payload_cb)
{
    pb_mp_DownlinkMessage msg;
    payload_cb(msg.payload, msg.which_payload);

    m_mutex.lock();
    msg.message_id = m_message_id++;
    // TODO: Fill in the rest of the downlink message data
    
    // TODO: Replace the log message string with a fixed size char
    // buffer in proto and then replace sizeof(...) with pb_mp_DownlinkMessage_size
    char out_buffer[sizeof(pb_mp_DownlinkMessage)];
    pb_ostream_t pb_ostream = pb_ostream_from_buffer((pb_byte_t*)out_buffer, sizeof(out_buffer));
    
    if (pb_encode(&pb_ostream, pb_mp_DownlinkMessage_fields, &msg)) {
        static auto write_cb = [this](ssize_t status) {
            m_mutex.unlock();
        };

        // If didn't start the write propertly, unlock the mutex for the next sender
        if (!m_tx_dev.write_async(out_buffer, pb_ostream.bytes_written, write_cb)) {
            m_mutex.unlock();
            return false;
        }
    } else {
        log_error("Failed to encode downlink message!");
        return false;
    }
    
    return true;
}

}