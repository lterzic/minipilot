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
rx::set_handler(payload_e payload, rx_handler& handler) noexcept
{
    if (m_handlers.find(payload) != m_handlers.end())
        return false;
    m_handlers[payload] = &handler;
    return true;
}

void
rx::run() noexcept
{
    while (true) {
        char recv_buf[MP_PB_LINK_UPLINK_SIZE];

        // Read status is negative in case the read operation failed, else
        // it is the size of the received message in bytes
        ssize_t read_status = -1;
        auto read_cb = [this, &read_status](ssize_t status) {
            read_status = status;
            notify_from_isr();
        };
        
        // Clear notifications if any exist
        wait_notification(milliseconds_t(0), true);

        if (!m_rx_dev.read_async(recv_buf, sizeof(recv_buf), read_cb)) {
            log_warning("rx read start error");
            // Receiver might be cleaning up from the last receive operation
            // so sleep to give some time to become ready
            sleep(RX_FAIL_SLEEP);
            continue;
        }

        // Wait for the next message for predefined time, if it's not
        // received, assume that the connection is lost
        if (!wait_notification(m_timeout.timeout)) {
            m_rx_dev.abort_async_read();
            
            // Notification must exist here since either the read finished
            // during the abort call, or the abort forced the callback
            // TODO: If abort specification is changed such that it doesn't
            // trigger the callback, the timeout here can be set to 0
            if (!wait_notification(milliseconds_t(1))) {
                log_error("rx read abort error");
            }

            // Read status -1 means that the operation was aborted, ie. timed out
            if (read_status == -1) {
                log_warning("rx timeout");
                m_timeout.cb();
                continue;
            }
        }

        if (read_status <= 0) {
            log_error("rx read error");
            continue;
        }

        pb::link::uplink_s recv_msg = {0};

        pb_istream_t buf_istream = pb_istream_from_buffer((const pb_byte_t*)recv_buf, read_status);
        if (pb_decode(&buf_istream, MP_PB_LINK_UPLINK_FIELDS, &recv_msg)) {
            // TODO (Optional): Add buffering (queue) for parsed messages
            // to decouple handling time from parsing time and call handler elsewhere

            // If a handler for this payload exists, run it in this thread's context
            auto handler = m_handlers.find(recv_msg.which_payload);
            if (handler != m_handlers.end())
                handler->second->handle(recv_msg.payload);
        } else {
            log_error("rx decode error");
        }
    }
}

}