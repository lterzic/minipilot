#include "telemetry/sysmon.hpp"
#include "common/chrono.hpp"
#include "common/pb.hpp"
#include <FreeRTOS.h>
#include <task.h>
#include <pb_encode.h>

namespace mp {

sysmon&
sysmon::get_instance() noexcept
{
    static sysmon s_sysmon;
    return s_sysmon;
}

void
sysmon::on_task_switch() noexcept
{
    auto* current_task = xTaskGetCurrentTaskHandle();

    pb::telemetry::task_switch_in_s task_switch {
        .task_id = static_cast<uint32_t>(uxTaskGetTaskNumber(current_task)),
        .has_timestamp = true,
        .timestamp = pb_from(get_time_since_start())
    };

    get_instance().m_switch_count++;
    get_instance().m_switches.push(task_switch);
}

bool
sysmon::produce(payload_u& payload) noexcept
{
    payload.kernel.switch_count = get_instance().m_switch_count;
    payload.kernel.task_switch_in.funcs.encode = &sysmon::encode_switches;
    payload.kernel.task_switch_in.arg = &sysmon::get_instance();
    return true;
}

bool
sysmon::encode_switches(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    sysmon& sysmon_instance = *reinterpret_cast<sysmon*>(*arg);
    
    pb::telemetry::task_switch_in_s task_switch;
    while (sysmon_instance.m_switches.pop(&task_switch)) {
        if (!pb_encode_tag_for_field(stream, field))
            return false;
        if (!pb_encode_submessage(stream, MP_PB_TELEMETRY_KERNEL_TASK_SWITCH_IN_FIELDS, &task_switch))
            return false;
    }

    return true;
}

void mp_on_task_switch()
{
    sysmon::get_instance().on_task_switch();
}

}