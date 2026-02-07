#include "link/tx.hpp"
#include "logging/logging.hpp"
#include <pb_encode.h>

namespace mp {

tx::tx(emblib::io::ostream& tx_dev) :
    static_task("tx", TX_TASK_PRIORITY),
    m_tx_dev(tx_dev),
    m_message_id(0)
{}

void tx::run() noexcept
{
    while (true) {
        // Wait indefinitely for a message from the queue
        tx_message_s* msg;
        m_queue.receive(msg, milliseconds_t(-1));

        // Fill in other message data as needed
        msg->message_id = m_message_id++;

        // Pre-allocated buffer for sending. Message can be also sent
        // as it is encoded, without pre allocating the buffer, but
        // this allows using DMA by the tx device
        pb_byte_t send_buf[256];
        pb_ostream_t pb_ostream = pb_ostream_from_buffer(send_buf, sizeof(send_buf));

        if (!pb_encode(&pb_ostream, PBLINK_LINK_DOWNLINK_FIELDS, msg)) {
            log_error("Failed to encode tx message");
            m_alloc.dealloc(msg);
            continue;
        }

        // Callback to notify this task once the async write
        // operation is finished
        auto write_cb = [this](ssize_t status) {
            notify_from_isr();
        };

        // Clear notifications in case the previous send was aborted,
        // and the notification arrived late
        wait_notification(milliseconds_t(0), true);

        if (!m_tx_dev.write_async((const char*)send_buf, pb_ostream.bytes_written, write_cb)) {
            log_error("Failed to write tx message");
            m_alloc.dealloc(msg);
            continue;
        }
        
        // Wait for a notification that the send was complete. In case
        // of a timeout, abort async write and discard this message.
        // Abort will generate another notification which will be cleared
        // by the next iteration.
        if (!wait_notification(TX_WRITE_TIMEOUT)) {
            m_tx_dev.abort_async_write();
            log_error("Write timeout");
        }
        m_alloc.dealloc(msg);
    }
}

}