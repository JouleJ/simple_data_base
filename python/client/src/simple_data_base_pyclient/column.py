from simple_data_base_pyclient.sender import Sender, Sendable
from simple_data_base_pyclient.core import ValueType


class Column(Sendable):
    def __init__(self, name: str, type_: ValueType) -> None:
        self.name = name
        self.type_ = type_

    def be_sent(self, sender: Sender) -> None:
        sender.send_str(self.name)
        self.type_.be_sent(sender)
