#include "rx.hpp"
#include "common/config.hpp"
#include "logging/logging.hpp"
#include <pb_decode.h>

namespace mp {

rx::rx(emblib::io::istream& rx_dev, timeout_s timeout) :
    static_task("rx", RX_TASK_PRIORITY),
    m_rx_dev(rx_dev),
    m_timeout(timeout)
{}

bool
rx::set_handler(pb_size_t payload_type, handler_cb cb) noexcept
{
    if (m_handlers.find(payload_type) != m_handlers.end())
        return false;
    m_handlers[payload_type] = cb;
    return true;
}

void
rx::run() noexcept
{
    while (true) {
        char recv_buf[sizeof(mp_pb_link_Uplink)];
        mp_pb_link_Uplink recv_msg = mp_pb_link_Uplink_init_zero;

        // Read status is negative in case the read operation failed, else
        // it is the size of the received message in bytes
        ssize_t read_status = -1;
        auto read_cb = [this, &read_status](ssize_t status) {
            read_status = status;
            notify_from_isr();
        };
        
        if (!m_rx_dev.read_async(recv_buf, sizeof(recv_buf), read_cb)) {
            log_warning("Receiver read start fail!");
            // Receiver might be cleaning up from the last receive operation
            // so sleep to give some time to become ready
            sleep(RX_FAIL_SLEEP);
            continue;
        }

        // Wait for the next message for predefined time, if it's not
        // received, assume that the connection is lost
        if (!wait_notification(m_timeout.timeout)) {
            m_timeout.cb();
            continue;
        }

        if (read_status <= 0) {
            log_error("Receiver read error!");
            continue;
        }

        pb_istream_t buf_istream = pb_istream_from_buffer((const pb_byte_t*)recv_buf, read_status);
        if (pb_decode(&buf_istream, mp_pb_link_Uplink_fields, &recv_msg)) {
            // TODO (Optional): Add buffering (queue) for parsed messages
            // to decouple handling time from parsing time and call handler elsewhere

            // If a handler for this payload exists, run it in this thread's context
            auto handler = m_handlers.find(recv_msg.which_payload);
            if (handler != m_handlers.end())
                handler->second(recv_msg);
        }
    }
}

}