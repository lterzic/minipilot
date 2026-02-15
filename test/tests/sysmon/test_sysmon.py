from pblink.telemetry.telemetry_pb2 import Telemetry
import time

TASK1_PERIOD_MS = 60
TASK2_PERIOD_MS = 70
SINK_PERIOD_MS = 500

def test_sysmon(firmware, device):
    assert device.receive() is None

    firmware.start()
    time.sleep(0.7)
    firmware.stop()

    # Empty telemetry message at the start since the sink
    # task has a higher priority than the periodic tasks
    telemetry_bytes = device.receive()
    assert telemetry_bytes is not None
    telemetry = Telemetry()
    telemetry.ParseFromString(telemetry_bytes)

    assert telemetry.channels[0].WhichOneof("payload") == "kernel"
    assert telemetry.channels[0].kernel.switch_count == 0

    # This should be the telemetry message sent at the second
    # iteration of the loop at 500ms after start
    telemetry_bytes = device.receive()
    assert telemetry_bytes is not None
    telemetry = Telemetry()
    telemetry.ParseFromString(telemetry_bytes)

    # One iteration of each task happened at start
    task1_count = 1
    task2_count = 1

    assert telemetry.channels[0].WhichOneof("payload") == "kernel"
    for switch in telemetry.channels[0].kernel.task_switch_in:
        if switch.task_id == 1:
            assert abs(switch.timestamp.ms - task1_count * TASK1_PERIOD_MS) <= 1
            task1_count += 1
        elif switch.task_id == 2:
            assert abs(switch.timestamp.ms - task2_count * TASK2_PERIOD_MS) <= 1
            task2_count += 1

    assert task1_count == SINK_PERIOD_MS // TASK1_PERIOD_MS + 1
    assert task2_count == SINK_PERIOD_MS // TASK2_PERIOD_MS + 1