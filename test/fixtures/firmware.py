import subprocess
import sys
import pytest

class Firmware:
    def __init__(self, test_name: str):
        self.test_name = test_name
        self.proc = None

    def get_exe_path(self):
        TEST_EXE_PREFIX = "mp_test_"
        TEST_EXE_PATH = "build/test/"
        py_path = sys.executable
        root_path = "/".join(py_path.split("/")[:-4])
        return root_path + "/" + TEST_EXE_PATH + TEST_EXE_PREFIX + self.test_name

    def start(self):
        if self.proc is not None:
            print("A process is already running. Stop it first.")
            return
        
        try:
            self.proc = subprocess.Popen([self.get_exe_path()])
            print(f"Process started with PID: {self.proc.pid}")
        except Exception as e:
            self.proc = None
            print(f"Failed to start process: {e}")

    def stop(self):
        if self.proc is None:
            print("No process is running.")
            return
        
        if self.proc.poll() is not None:
            self.proc.wait()
            self.proc = None
            print("Process has already terminated.")
            return
        
        try:
            self.proc.terminate()
            try:
                self.proc.wait(timeout=1)
                print("Process terminated gracefully.")
            except subprocess.TimeoutExpired:
                print("Process didn't terminate gracefully, forcing kill...")
                self.proc.kill()
                self.proc.wait()
                print("Process killed.")
        except Exception as e:
            print(f"Failed to stop process: {e}")
        self.proc = None

@pytest.fixture
def firmware(info):
    return Firmware(info.test_name)