from typing import List, Self
import socket

from simple_data_base_pyclient.sender import Sender


class Connection(Sender):
    def __init__(self, address: str, port: int) -> None:
        self.address = address
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def __do_connect(self) -> None:
        self.socket.connect((self.address, self.port))

    def send(self, data: bytes) -> None:
        self.socket.sendall(data)

    def __do_close(self) -> None:
        self.socket.close()

    def __enter__(self) -> Self:
        self.__do_connect()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.__do_close()


class BufferedSender(Sender):
    def __init__(self, connection: Connection, max_fragmentation: int = 8) -> None:
        self.connection = connection
        self.buffer: List[bytes] = []
        self.max_fragmentation = max_fragmentation

    def send(self, data: bytes) -> None:
        self.buffer.append(data)
        if len(self.buffer) > self.max_fragmentation:
            self.buffer = [b"".join(self.buffer)]
            self.max_fragmentation += self.max_fragmentation

    def flush(self) -> None:
        if self.buffer:
            data = b"".join(self.buffer)
            self.buffer = []
            self.connection.send(data)

    def __enter__(self) -> Self:
        self.flush()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.flush()
