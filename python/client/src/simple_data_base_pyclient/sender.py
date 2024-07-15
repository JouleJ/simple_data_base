from abc import ABC, abstractmethod


class Sender(ABC):
    PLUS: bytes = b"\x81"
    MINUS: bytes = b"\x82"
    ZERO: bytes = b"\x83"
    TRUE: bytes = b"\xFF"
    FALSE: bytes = b"\x00"

    @abstractmethod
    def send(self, data: bytes) -> None:
        raise NotImplementedError()

    def send_int(self, value: int) -> None:
        last_token: bytes = (
            self.PLUS if value > 0 else (self.MINUS if value < 0 else self.ZERO)
        )

        value = abs(value)
        while value > 0:
            digit: int = value & 0x7F
            value >>= 7

            self.send(digit.to_bytes())

        self.send(last_token)

    def send_size(self, value: int) -> None:
        assert value >= 0, "Size ought to be positive"
        while value > 0:
            digit: int = value & 0x7F
            value >>= 7

            if value <= 0:
                digit |= 0x80

            self.send(digit.to_bytes())

    def send_bool(self, value: bool) -> None:
        if value:
            self.send(self.TRUE)
        else:
            self.send(self.FALSE)

    def send_str(self, value: str, encoding="utf-8") -> None:
        self.send_size(len(value))
        self.send(value.encode(encoding))


class Sendable(ABC):
    @abstractmethod
    def be_sent(self, sender: Sender) -> None:
        raise NotImplementedError()
