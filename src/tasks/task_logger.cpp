#include "task_logger.hpp"
#include <cstring>

namespace mp {

using milliseconds_t;

ssize_t task_logger::write(const char* data, size_t size, milliseconds_t timeout) noexcept
{
    log_msg_s msg;
    msg.length = size;
    memcpy(msg.data, data, size);
    return m_log_msg_queue.send(msg, timeout) ? size : -1;
}

void task_logger::run() noexcept
{
    assert(m_log_device.probe(milliseconds_t(0)));
    bool use_async = m_log_device.is_async_available();

    log_msg_s recv_msg;
    while (true) {
        // TODO: Bypass this copying by reading the queue's top element data
        // and removing the item from queue after write somehow
        m_log_msg_queue.receive(recv_msg, emblib::MILLISECONDS_MAX);

        if (use_async) {
            bool status = m_log_device.write_async(recv_msg.data, recv_msg.length, [this](ssize_t status) {
                notify_from_isr();
            });
            
            if (status)
                wait_notification(emblib::MILLISECONDS_MAX);
        } else {
            m_log_device.write(recv_msg.data, recv_msg.length, milliseconds_t(0));
        }
    }
}

}