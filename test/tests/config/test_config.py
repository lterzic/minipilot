from pblink.config.config_pb2 import Config
from pblink.config.host_pb2 import ConfigHostRequest, ConfigHostAck, ConfigHostUpdate
import time

def test_config(firmware, device, config):
    # Unused config fixture is needed to create an empty file
    # on test start

    assert device.receive() is None

    firmware.start()
    
    # Firmware should try to load the config from storage and
    # fail, then request action from the host
    request_bytes = device.receive(0.5)
    assert request_bytes is not None
    request = ConfigHostRequest()
    request.ParseFromString(request_bytes)

    assert request.current_config == Config()
    assert request.request_timeout.ms == 3000

    # Acknowledge received request, can be replaced with sending anything
    ack = ConfigHostAck()
    device.send(ack.SerializeToString())

    dummy_key = 123456789123456789
    new_config = Config()
    new_config.link.shared_key = dummy_key

    update = ConfigHostUpdate()
    update.config.CopyFrom(new_config)
    update.complete = True
    device.send(update.SerializeToString())
    time.sleep(0.5)
    firmware.stop()

    # Start the firmware again, this time there is a saved configuration
    firmware.start()

    # Firmware should now try to load the config from storage
    # and succeed, then send the loaded config to the host
    request_bytes = device.receive(0.5)
    assert request_bytes is not None
    firmware.stop()

    request = ConfigHostRequest()
    request.ParseFromString(request_bytes)
    assert request.current_config.link.shared_key == dummy_key
