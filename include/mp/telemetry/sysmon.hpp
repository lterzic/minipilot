#pragma once

#include "producer.hpp"
#include <emblib/lockfree/spmc_queue.hpp>

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
    void on_task_switch() noexcept;
    
    /**
     * Kernel produce
     */
    bool produce(payload_u& payload) noexcept override;

private:
    sysmon();

    /**
     * Consume current switch queue entries into task switch telemetry
     */
    static bool encode_switches(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);

private:
    using switch_queue = emblib::lockfree::spmc_queue<pblink::telemetry::task_switch_in_s, 64>;
    
    switch_queue m_switch_queue;
    switch_queue::reader<false> m_main_reader;
    switch_queue::reader<false> m_temp_reader;

    uint32_t m_prev_task;
};

}