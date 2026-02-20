import pblink.config.config_pb2
import os
import pytest

class Config:
    def __init__(self, config_path: str):
        self.config = pblink.config.config_pb2.Config()
        self.path = config_path
        open(config_path, 'w').close()

    def get_config(self):
        return self.config

    def save(self):
        serial = self.config.SerializeToString()
        file = open(self.path, "w")
        file.write(serial)
        file.close()

@pytest.fixture
def config(info, tmp_path):
    return Config(tmp_path / "config.bin")