#include "task_receiver.hpp"
#include "util/logger.hpp"
#include <pb_decode.h>

namespace mp {

task_receiver::task_receiver(emblib::char_dev& receiver_device) noexcept :
    task("Task Receiver", TASK_RECEIVER_PRIORITY, m_task_stack),
    m_receiver_device(receiver_device)
{
    m_pb_istream.callback = &task_receiver::pb_istream_cb;
    m_pb_istream.state = this;
    m_pb_istream.bytes_left = SIZE_MAX;
}

bool task_receiver::get_command(mp_pb_Command& command_buffer) noexcept
{
    // Try to read from queue with timeout 0
    // If the queue is empty it will return false
    return m_command_queue.receive(command_buffer, emblib::ticks_t(0));
}

bool task_receiver::pb_istream_cb(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    // If `buf` is NULL, then the next `count` bytes are not needed
    // This is static since it doesn't have to be thread-safe as this
    // data is never used, and the size is chosen to be not to small
    // to increase overhead, and not too big to waste memory
    static char discard_buffer[64];

    // This callback is always called from the context of the task
    task_receiver* this_task = (task_receiver*)stream->state;

    // Callback used when task's receiver finishes
    ssize_t recv_status = -1;
    auto read_async_cb = [this_task, &recv_status](ssize_t status) {
        recv_status = status;
        this_task->notify_from_isr();
    };

    if (buf == NULL) {
        while (count > 0) {
            size_t to_recv = count > sizeof(discard_buffer) ? sizeof(discard_buffer) : count;
            
            // If failed starting the read op, exit
            if (!this_task->m_receiver_device.read_async(discard_buffer, to_recv, read_async_cb))
                return false;
            // This is okay since we know we are in the context of this task
            this_task->wait_notification();

            if (recv_status <= 0)
                return false;
            else
                count -= recv_status;
        }
        return true;
    }

    // Try to start an async read and wait for it to finish
    if (!this_task->m_receiver_device.read_async((char*)buf, count, read_async_cb))
        return false;

    this_task->wait_notification();
    return recv_status == count;
}

void task_receiver::run() noexcept
{
    assert(m_receiver_device.is_async_available());

    while (true) {
        mp_pb_Command recv_command = mp_pb_Command_init_zero;

#if TASK_RECEIVER_STREAM_DATA
        if (pb_decode(&m_pb_istream, mp_pb_Command_fields, &recv_command)) {
            // Send to queue with infinite timeout
            m_command_queue.send(recv_command);
        } else {
            log_error("Receiver decoding failed!");
            // Decode process (probably reading) was not successful, go
            // to sleep for some time
            sleep(std::chrono::milliseconds(100));
        }
#else
        char recv_buf[sizeof(mp_pb_Command)];

        ssize_t recv_status = -1;
        bool start_status = m_receiver_device.read_async(recv_buf, sizeof(recv_buf), [this, &recv_status](ssize_t status){
            recv_status = status;
            notify_from_isr();
        });
        
        if (!start_status) {
            log_warning("Receiver read start fail!");
            // Sleep to give time to the receiver to unblock
            sleep(std::chrono::milliseconds(100));
            continue;
        }

        wait_notification();
        if (recv_status <= 0) {
            log_error("Receiver read error!");
            continue;
        }

        pb_istream_t buf_istream = pb_istream_from_buffer((const pb_byte_t*)recv_buf, recv_status);
        if (pb_decode(&buf_istream, mp_pb_Command_fields, &recv_command)) {
            m_command_queue.send(recv_command);
        }
    }
#endif
}

}