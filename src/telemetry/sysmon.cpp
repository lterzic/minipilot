#include "telemetry/sysmon.hpp"
#include "common/pb.hpp"
#include <FreeRTOS.h>
#include <task.h>
#include <pb_encode.h>

namespace mp {

sysmon::sysmon() :
    m_main_reader(m_switch_queue.get_reader<false>()),
    m_temp_reader(m_main_reader)
{}

sysmon&
sysmon::get_instance() noexcept
{
    static sysmon s_sysmon;
    return s_sysmon;
}

void
sysmon::on_task_switch() noexcept
{
    auto current_task = xTaskGetCurrentTaskHandle();
    auto task_id = static_cast<uint32_t>(uxTaskGetTaskNumber(current_task));

    if (task_id == m_prev_task)
        return;

    m_switch_queue.emplace(
        task_id,
        true,
        pblink::time_s {
            .which_units = pblink::time_s::units_e::MS,
            .units = xTaskGetTickCount()
        }
    );

    m_prev_task = task_id;
}

bool
sysmon::produce(payload_u& payload) noexcept
{
    m_main_reader = m_temp_reader;
    
    payload.kernel.switch_count = m_switch_queue.get_total_count();
    payload.kernel.task_switch_in.funcs.encode = &sysmon::encode_switches;
    payload.kernel.task_switch_in.arg = &sysmon::get_instance();
    return true;
}

bool
sysmon::encode_switches(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    sysmon& sysmon_instance = *reinterpret_cast<sysmon*>(*arg);
    sysmon_instance.m_temp_reader = sysmon_instance.m_main_reader;
    
    pblink::telemetry::task_switch_in_s task_switch;
    while (sysmon_instance.m_temp_reader.read(task_switch)) {
        if (!pb_encode_tag_for_field(stream, field))
            return false;
        if (!pb_encode_submessage(stream, PBLINK_TELEMETRY_KERNEL_TASK_SWITCH_IN_FIELDS, &task_switch))
            return false;
    }

    return true;
}

}