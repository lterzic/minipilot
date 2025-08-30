#include "rx.hpp"
#include "common/config.hpp"
#include "logging/logging.hpp"
#include <pb_decode.h>

namespace mp {

rx::rx(emblib::io::istream& rx_dev) :
    static_task("PB rx task", RX_TASK_PRIORITY),
    m_rx_dev(rx_dev)
{}

void rx::run()
{
    while (true) {
        char recv_buf[sizeof(mp_pb_link_UplinkMessage)];
        mp_pb_link_UplinkMessage recv_msg = mp_pb_link_UplinkMessage_init_zero;

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

        // Wait infinitely for received data
        wait_notification(milliseconds_t(-1));
        if (read_status <= 0) {
            log_error("Receiver read error!");
            continue;
        }

        pb_istream_t buf_istream = pb_istream_from_buffer((const pb_byte_t*)recv_buf, read_status);
        if (pb_decode(&buf_istream, mp_pb_link_UplinkMessage_fields, &recv_msg)) {
            // TODO (Optional): Add buffering (queue) for parsed messages
            // to decouple handling time from parsing time and call handler elsewhere

            // If a handler for this payload exists, run it in this thread's context
            auto handler = m_handlers.find(recv_msg.which_payload);
            if (handler != m_handlers.end())
                handler->second->handle(recv_msg);
        }
    }
}

}