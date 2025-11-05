#pragma once

#include "pb/link/link.nanopb.h"
#include <emblib/io/ostream.hpp>
#include <emblib/rtos/mutex.hpp>
#include <emblib/rtos/semaphore.hpp>
#include <etl/delegate.h>

namespace mp {

/**
 * Transmitter
 * 
 * Creates final output (downlink) messages from the provided payloads and
 * manages multiple writers accessing the underlying transmit serial device.
 * All of the processing of the output messages is done in the calling
 * task's context.
 * 
 * @todo Make this a task and instead of holding a mutex while writing,
 * copy the downlink message into a queue and then encode and send from
 * this task's context using async write and waiting for notification
 */
class tx {
public:
    /**
     * @todo Add system information like vehicle id, session id,
     * etc. so that it can be sent in downlink messages
     */
    explicit tx(emblib::io::ostream& tx_dev) noexcept;

    /**
     * Send a downlink message
     * @note Blocking - this method processes the message and starts
     * an async write, then blocks the calling task's context until
     * the write operation completes
     * @todo Instead of encoding and sending the message in
     * the caller context, can convert this to a task and copy
     * to a queue, then process in this task's context
     */
    bool send_downlink(mp_pb_link_Downlink& msg) noexcept;

    /**
     * Helper method for a specific payload type
     * @note Implemented for each payload using template specialization
     */
    template <typename payload_type>
    bool send_downlink(etl::delegate<void(payload_type&)>&& payload_cb) noexcept;

    /**
     * Prevent TX from sending messages until `tx::resume` is called
     */
    void suspend() noexcept
    {
        emblib::rtos::scoped_lock lock(m_mutex);
        m_suspend = true;
    }

    /**
     * Allow sending messages
     */
    void resume() noexcept
    {
        emblib::rtos::scoped_lock lock(m_mutex);
        m_suspend = false;
    }

private:
    // Message counter
    size_t m_message_id;
    // If this is true, messages can't be sent
    bool m_suspend;

    // Serial data transmit device
    emblib::io::ostream& m_tx_dev;
    // Mutex for ensuring a single user
    emblib::rtos::mutex m_mutex;
    // Semaphore to signal the end of the async write
    emblib::rtos::semaphore m_write_smphr;
};

template <>
inline bool tx::send_downlink(etl::delegate<void(mp_pb_telemetry_Downlink&)>&& payload_cb) noexcept
{
    mp_pb_link_Downlink msg = {0};
    msg.which_payload = mp_pb_link_Downlink_telemetry_tag;
    payload_cb(msg.payload.telemetry);
    return send_downlink(msg);
}

template <>
inline bool tx::send_downlink(etl::delegate<void(mp_pb_logging_Downlink&)>&& payload_cb) noexcept
{
    mp_pb_link_Downlink msg = {0};
    msg.which_payload = mp_pb_link_Downlink_logging_tag;
    payload_cb(msg.payload.logging);
    return send_downlink(msg);
}

}