import pytest
import subprocess

class Info:
    def __init__(self, test_name):
        self.test_name = test_name
        self.fw_name = "mp_test_" + self.test_name
        self.fw_dir = Info.get_root_dir() + "/build/test"
        self.fw_path = self.fw_dir + "/" + self.fw_name

    def get_root_dir():
        res = subprocess.run(["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True).stdout
        return res.strip()

@pytest.fixture
def info(request):
    return Info(request.path.parent.name)