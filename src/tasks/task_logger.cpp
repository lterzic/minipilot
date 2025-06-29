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

        bool status = m_log_device.write_async(recv_msg.data, recv_msg.length, [this](ssize_t status) {
            notify_from_isr();
        });
        
        if (status)
            wait_notification(milliseconds_t(-1));
    }
}

}