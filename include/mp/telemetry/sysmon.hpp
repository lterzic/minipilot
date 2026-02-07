#pragma once

#include "producer.hpp"
#include <emblib/lockfree/spsc_queue.hpp>

namespace mp {

class sysmon : public telemetry_producer<telemetry_channel_e::KERNEL> {
public:
    /**
     * Get the singleton instance
     */
    static sysmon& get_instance() noexcept;

    /**
     * Task switch handler
     */
    static void on_task_switch() noexcept;
    
    /**
     * Kernel produce
     */
    bool produce(payload_u& payload) noexcept override;

private:
    sysmon() = default;

    /**
     * Consume current switch queue entries into task switch telemetry
     */
    static bool encode_switches(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);

private:
    std::atomic_size_t m_switch_count = 0;
    emblib::lockfree::spsc_queue<pblink::telemetry::task_switch_in_s, 64> m_switches;
};

}