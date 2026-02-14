from pblink.logging.log_pb2 import Log, LogLevel
import pytest
import time

def test_logging(firmware, device):
    assert device.receive() is None
    
    firmware.start()
    time.sleep(1)
    firmware.stop()

    # First log is before the scheduler is started
    log_bytes = device.receive()
    assert log_bytes is not None
    log = Log()
    log.ParseFromString(log_bytes)

    assert log.level == LogLevel.LOG_LEVEL_INFO
    assert log.format == "Pre scheduler start log with two args: {}, {}"
    assert len(log.args) == 2
    
    assert log.args[0].WhichOneof("value") == "int32v"
    assert log.args[1].WhichOneof("value") == "floatv"

    assert log.args[0].int32v == -12345
    assert log.args[1].floatv == pytest.approx(123.456)

    # Second log is after the scheduler starts, from a task
    log_bytes = device.receive()
    assert log_bytes is not None
    log = Log()
    log.ParseFromString(log_bytes)

    assert log.level == LogLevel.LOG_LEVEL_WARNING
    assert log.format == "Logging from a task with 1 argument: {}"
    assert len(log.args) == 1

    assert log.args[0].WhichOneof("value") == "stringv"
    assert log.args[0].stringv == "My string argument"
