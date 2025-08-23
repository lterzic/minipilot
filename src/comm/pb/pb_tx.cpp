#include "pb_tx.hpp"
#include "logging/logging.hpp"
#include <pb_encode.h>

namespace mp {

pb_tx::pb_tx(emblib::io::ostream<char>& tx_dev) :
    m_tx_dev(tx_dev)
{}

bool pb_tx::send_downlink(payload_cb_t payload_cb)
{
    mp_pb_link_DownlinkMessage msg;
    payload_cb(msg.payload, msg.which_payload);

    emblib::scoped_lock lock(m_mutex);
    msg.message_id = m_message_id++;
    // TODO: Fill in the rest of the downlink message data
    
    // TODO: Replace the log message string with a fixed size char
    // buffer in proto and then replace sizeof(...) with pb_mp_DownlinkMessage_size
    // Here we assume that sizeof(pb_mp_Log) < sizeof(pb_mp_Telemetry) so no extra
    // size is added to this buffer
    char out_buffer[sizeof(mp_pb_link_DownlinkMessage)];
    pb_ostream_t pb_ostream = pb_ostream_from_buffer((pb_byte_t*)out_buffer, sizeof(out_buffer));
    
    if (pb_encode(&pb_ostream, mp_pb_link_DownlinkMessage_fields, &msg)) {
        auto write_cb = [this](ssize_t status) {
            m_write_smphr.signal_from_isr();
        };

        if (!m_tx_dev.write_async(out_buffer, pb_ostream.bytes_written, write_cb)) {
            return false;
        }
        
        // If the async write doesn't finish in the given time, abort the operation
        if (!m_write_smphr.wait(TX_WRITE_TIMEOUT)) {
            m_tx_dev.abort_async_write();
            log_error("Failed to send downlink message!");
            return false;
        }
    } else {
        log_error("Failed to encode downlink message!");
        return false;
    }
    
    return true;
}

}