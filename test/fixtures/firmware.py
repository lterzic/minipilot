import subprocess
import pytest

class Firmware:
    def __init__(self, binary_path, tmp_path):
        self.binary_path = binary_path
        self.tmp_path = tmp_path
        self.proc = None

    def start(self):
        if self.proc is not None:
            print("A process is already running. Stop it first.")
            return
        
        try:
            self.proc = subprocess.Popen([self.binary_path], cwd=self.tmp_path)
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
def firmware(info, tmp_path):
    return Firmware(info.fw_path, tmp_path)