from typing import Any, List, Type, TypeAlias, Self
from abc import abstractmethod
from enum import Enum

from simple_data_base_pyclient.sender import Sender, Sendable


ValueContent: TypeAlias = int | str | bool


class TypeKind(Enum):
    INTEGER: int = 1
    VARCHAR: int = 2
    BOOLEAN: int = 3


class ValueType(Sendable):
    @abstractmethod
    def get_name(self) -> str:
        raise NotImplementedError()

    @abstractmethod
    def get_native_type(self) -> Type[Any]:
        raise NotImplementedError()


class IntegerType(ValueType):
    def get_name(self) -> str:
        return "integer"

    def get_native_type(self) -> Type[Any]:
        return int

    def be_sent(self, sender: Sender) -> None:
        sender.send_size(TypeKind.INTEGER.value)


class VarcharType(ValueType):
    def get_name(self) -> str:
        return "varchar"

    def get_native_type(self) -> Type[Any]:
        return str

    def be_sent(self, sender: Sender) -> None:
        sender.send_size(TypeKind.VARCHAR.value)


class BooleanType(ValueType):
    def get_name(self) -> str:
        return "boolean"

    def get_native_type(self) -> Type[Any]:
        return bool

    def be_sent(self, sender: Sender) -> None:
        sender.send_size(TypeKind.BOOLEAN.value)


class Value(Sendable):
    INTEGER_TYPE: ValueType = IntegerType()
    VARCHAR_TYPE: ValueType = VarcharType()
    BOOLEAN_TYPE: ValueType = BooleanType()

    def __init__(self, content: ValueContent):
        self.content = content

    def __str__(self) -> str:
        return f"Value({self.content!r})"

    def get(self) -> ValueContent:
        return self.content

    def get_type(self) -> ValueType:
        if isinstance(self.get(), int):
            return self.INTEGER_TYPE

        if isinstance(self.get(), str):
            return self.VARCHAR_TYPE

        if isinstance(self.get(), bool):
            return self.BOOLEAN_TYPE

        raise ValueError(f"Cannot determine type of {self}")

    def be_sent(self, sender: Sender) -> None:
        self.get_type().be_sent(sender)

        if isinstance(self.content, int):
            sender.send_int(self.content)
        elif isinstance(self.content, str):
            sender.send_str(self.content)
        elif isinstance(self.content, bool):
            sender.send_bool(self.content)
        else:
            raise ValueError(f"Cannot send {self}")


class Row(Sendable):
    def __init__(self, values: List[Value]) -> None:
        self.values = values

    @classmethod
    def make(cls, *args: ValueContent) -> Self:
        return cls([Value(arg) for arg in args])

    def __str__(self) -> str:
        return "Row(" + ", ".join(f'{value.content!r}' for value in self.values) + ")"

    def be_sent(self, sender: Sender) -> None:
        sender.send_size(len(self.values))
        for value in self.values:
            value.be_sent(sender)
