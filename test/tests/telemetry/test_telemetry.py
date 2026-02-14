from pblink.telemetry.telemetry_pb2 import Telemetry
import pytest
import time

MESSAGE_COUNT = 4
PERIOD = 0.5
START_DELAY = 0.1

def compare_vector(pb, test):
    return (pb.x == pytest.approx(test[0]) and
            pb.y == pytest.approx(test[1]) and
            pb.z == pytest.approx(test[2]))

def test_telemetry(firmware, device):
    assert device.receive() is None

    # Give the firmware time to generate all of the messages
    firmware.start()
    time.sleep((MESSAGE_COUNT - 1) * PERIOD + START_DELAY)
    firmware.stop()

    for i in range(MESSAGE_COUNT):
        telemetry_bytes = device.receive()
        assert telemetry_bytes is not None
        telemetry = Telemetry()
        telemetry.ParseFromString(telemetry_bytes)

        assert telemetry.timestamp.WhichOneof("units") == "ms"
        assert telemetry.timestamp.ms / 1000 <= i * PERIOD + START_DELAY
        assert len(telemetry.channels) == 2

        assert telemetry.channels[0].WhichOneof("payload") == "accelerometer"
        assert telemetry.channels[0].accelerometer.HasField("raw_acceleration") == False
        assert compare_vector(telemetry.channels[0].accelerometer.acceleration, [1, 2, 3])

        assert telemetry.channels[1].WhichOneof("payload") == "barometer"
        assert telemetry.channels[1].barometer.air_pressure == pytest.approx(123.456)
        assert telemetry.channels[1].barometer.air_temperature == pytest.approx(34.567)
