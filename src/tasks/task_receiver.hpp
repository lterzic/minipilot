#pragma once

#include "task_config.hpp"
#include "pb/command.pb.h"
#include <emblib/driver/char_dev.hpp>
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/queue.hpp>
#include <pb_decode.h>

namespace mp {

class task_receiver : public emblib::task {
public:
    task_receiver(emblib::char_dev& receiver_device) noexcept;

    /**
     * Returns true if a command was available and was successfully copied
     * into the provided buffer
     */
    bool get_command(mp_pb_Command& command_buffer) noexcept;

private:
    /**
     * Callback for receiving data as it is needed (we do not
     * receive a full buffer of data ahead of starting the decoding)
     * 
     * This is called during the execution of nanopb's pb_decode function
     */
    static bool pb_istream_cb(pb_istream_t *stream, uint8_t *buf, size_t count);

    /**
     * Implementation of the task
     */
    void run() noexcept override;

private:
    emblib::task_stack_t<TASK_RECEIVER_STACK_SIZE> m_task_stack;
    emblib::queue<mp_pb_Command, TASK_RECEIVER_QUEUE_SIZE> m_command_queue;
    emblib::char_dev& m_receiver_device;

    pb_istream_t m_pb_istream;
};

}