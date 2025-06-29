#include "task_logger.hpp"
#include <cstring>

namespace mp {

ssize_t task_logger::write(const char* data, size_t size, milliseconds_t timeout) noexcept
{
    log_msg_s msg;
    msg.length = size;
    memcpy(msg.data, data, size);
    return m_log_msg_queue.send(msg, timeout) ? size : -1;
}

void task_logger::run() noexcept
{
    log_msg_s recv_msg;
    while (true) {
        // TODO: Bypass this copying by reading the queue's top element data
        // and removing the item from queue after write somehow
        m_log_msg_queue.receive(recv_msg, milliseconds_t(-1));

        auto write_cb = [this](ssize_t status) {
            notify_from_isr();
        };

        // TODO: Add aborting in case of notification timeout
        if (m_log_device.write_async(recv_msg.data, recv_msg.length, write_cb))
            wait_notification(milliseconds_t(-1));
    }
}

}