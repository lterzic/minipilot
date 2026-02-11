#include "telemetry/sink.hpp"
#include "telemetry/producer.hpp"
#include "common/pb.hpp"
#include "common/config.hpp"

namespace mp {

telemetry_sink::telemetry_sink(milliseconds_t period) :
    static_task("telemetry_sink", TELEMETRY_TASK_PRIORITY),
    m_period(period)
{}

bool
telemetry_sink::subscribe(producer_id producer) noexcept
{
    return m_subscriptions.insert(producer).second;
}

void
telemetry_sink::run() noexcept
{
    while (true) {
        pblink::telemetry::telemetry_s msg = {0};
        msg.timestamp = pb_from(get_time_since_start());
        msg.has_timestamp = true;
        
        const auto& producer_map =
            telemetry_producer_registry::get_instance().get_producers();
        
        for (const producer_id& producer : m_subscriptions) {
            const auto channel_it = producer_map.find(producer.channel);
            if (channel_it == producer_map.end() ||
                channel_it->second.size() <= producer.source_idx)
                continue;

            if (channel_it->second[producer.source_idx]->produce(
                msg.channels[msg.channels_count].payload
            )) {
                msg.channels[msg.channels_count].source_id = producer.source_idx;
                msg.channels_count++;
            }
        }

        write(msg);
        sleep_periodic(m_period);
    }
}

}