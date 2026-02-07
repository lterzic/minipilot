#pragma once

#include "common/config.hpp"
#include <pblink/link/link.nanopb.h>
#include <emblib/io/ostream.hpp>
#include <emblib/lockfree/allocator.hpp>
#include <emblib/rtos/queue.hpp>
#include <emblib/rtos/task.hpp>

namespace mp {

/**
 * Transmitter
 * 
 * Creates final output (downlink) messages from the provided payloads and
 * manages multiple writers accessing the underlying transmit serial device.
 * All of the processing of the output messages is done in the calling
 * task's context.
 */
class tx : public emblib::rtos::static_task<1024> {
public:
    /**
     * Message type sent by the transmitter
     */
    using tx_message_s = pblink::link::downlink_s;

public:
    /**
     * @todo Add system information like vehicle id, session id,
     * etc. so that it can be sent in downlink messages
     */
    explicit tx(emblib::io::ostream& tx_dev) noexcept;

    /**
     * Init and enqueue a message for sending.
     * @param assign_lambda Lambda of type `void (tx_message_s&)` which
     * should be used to fill the appropriate payload for the message.
     * @returns `true` if message successfully enqueued for sending.
     */
    template <typename assign_lambda_type>
    bool send(assign_lambda_type&& assign_lambda) noexcept;

private:
    /**
     * Block until there is a message to be sent in the queue.
     * Once available, encode and send over the tx device.
     */
    void run() noexcept override;

private:
    // Message counter
    size_t m_message_id;
    // Serial data transmit device
    emblib::io::ostream& m_tx_dev;
    // Zero-copy queue using lock-free allocation and pointer queue
    emblib::lockfree::allocator<tx_message_s, TX_QUEUE_SIZE> m_alloc;
    emblib::rtos::queue<tx_message_s*, TX_QUEUE_SIZE> m_queue;
};

/**
 * Try to allocate data for the message and fill the payload. Rest
 * of the message data is filled once the message is next in queue
 * to be sent.
 */
template <typename assign_lambda_type>
inline bool tx::send(assign_lambda_type &&assign_lambda) noexcept
{
    if (tx_message_s* msg = m_alloc.alloc()) {
        *msg = {0};
        assign_lambda(*msg);
        return m_queue.send(msg, milliseconds_t(0));
    }
    return false;
}

}