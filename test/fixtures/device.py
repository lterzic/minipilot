import pytest
import socket

FW_IP = "127.0.0.1"
HOST_PORT = 26200
FW_PORT = 26201

class Device:
    def __init__(self):
        self.send_address = (FW_IP, FW_PORT)
        self.recv_address = (FW_IP, HOST_PORT)

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(self.recv_address)

    def send(self, data: bytes):
        self.sock.sendto(data, self.send_address)

    def receive(self, timeout = 0.0):
        self.sock.settimeout(timeout)
        try:
            data, addr = self.sock.recvfrom(1024)
            assert addr == self.send_address
            return data
        except socket.timeout:
            return None
        except BlockingIOError:
            return None

    def close(self):
        self.sock.close()

@pytest.fixture
def device():
    return Device()