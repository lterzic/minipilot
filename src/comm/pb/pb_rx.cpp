#include "pb_rx.hpp"
#include "mp_config.hpp"
#include "logging/logging.hpp"
#include <pb_decode.h>

namespace mp {

pb_rx::pb_rx(emblib::io::istream<char>& rx_dev) :
    task("PB rx task", RX_TASK_PRIORITY, m_stack),
    m_rx_dev(rx_dev)
{}

void pb_rx::run()
{
    while (true) {
        char recv_buf[sizeof(pb_mp_UplinkMessage)];
        pb_mp_UplinkMessage recv_msg = pb_mp_UplinkMessage_init_zero;

        // Read status is negative in case the read operation failed, else
        // it is the size of the received message in bytes
        ssize_t read_status = -1;
        auto read_cb = [this, &read_status](ssize_t status) {
            read_status = status;
            notify_from_isr();
        };
        
        if (!m_rx_dev.read_async(recv_buf, sizeof(recv_buf), read_cb)) {
            log_warning("Receiver read start fail!");
            // Sleep to give time to the receiver to unblock
            sleep(milliseconds_t(100));
            continue;
        }

        // Wait infinitely for received data
        wait_notification(milliseconds_t(-1));
        if (read_status <= 0) {
            log_error("Receiver read error!");
            continue;
        }

        pb_istream_t buf_istream = pb_istream_from_buffer((const pb_byte_t*)recv_buf, read_status);
        if (pb_decode(&buf_istream, pb_mp_UplinkMessage_fields, &recv_msg)) {
            // TODO (Optional): Add buffering for parsed messages
            m_handlers[recv_msg.which_payload](recv_msg);
        }
    }
}

}