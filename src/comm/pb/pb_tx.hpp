#pragma once

#include "pb/link/link.nanopb.h"
#include <emblib/io/ostream.hpp>
#include <emblib/rtos/mutex.hpp>
#include <emblib/rtos/semaphore.hpp>
#include <etl/delegate.h>

namespace mp {

/**
 * @todo Make this a task and instead of holding a mutex while writing,
 * copy the downlink message into a queue and then encode and send from
 * this task's context using async write and waiting for notification
 */
class pb_tx {
public:
    /**
     * Callback for filling in the payload data
     * First argument is the union for all payload types,
     * the second is the payload type selector
     */
    using payload_cb_t = etl::delegate<void(
        mp_pb_link_DownlinkMessage::_mp_pb_link_DownlinkMessage_payload&,
        pb_size_t&
    )>;

public:
    /**
     * @todo Add system information like vehicle id, session id,
     * etc. so that it can be sent in downlink messages
     */
    explicit pb_tx(emblib::io::ostream& tx_dev) noexcept;

    /**
     * Send a downlink message
     * @note Blocking
     * @todo Instead of encoding and sending the message in
     * the caller context, can convert this to a task and copy
     * to a queue, then process in this task's context
     */
    bool send_downlink(payload_cb_t payload_cb) noexcept;

private:
    // Message counter
    size_t m_message_id;

    // Serial data transmit device
    emblib::io::ostream& m_tx_dev;
    // Mutex for ensuring a single user
    emblib::rtos::mutex m_mutex;
    // Semaphore to signal the end of the async write
    emblib::rtos::semaphore m_write_smphr;
};

}