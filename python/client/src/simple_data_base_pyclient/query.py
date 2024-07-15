from enum import Enum
from typing import List

from simple_data_base_pyclient.column import Column
from simple_data_base_pyclient.core import Row
from simple_data_base_pyclient.sender import Sender, Sendable


class NodeKind(Enum):
    CREATE_TABLE = 1
    SELECT = 2
    INSERT = 3


class Query(Sendable):
    pass


class CreateTableCommand(Query):
    def __init__(
        self, schema_name: str, table_name: str, columns: List[Column]
    ) -> None:
        self.schema_name = schema_name
        self.table_name = table_name
        self.columns = columns

    def be_sent(self, sender: Sender) -> None:
        sender.send_size(NodeKind.CREATE_TABLE.value)

        sender.send_str(self.schema_name)
        sender.send_str(self.table_name)

        sender.send_size(len(self.columns))
        for column in self.columns:
            column.be_sent(sender)


class SelectCommand(Query):
    def __init__(
        self, schema_name: str, table_name: str, column_names: List[str]
    ) -> None:
        self.schema_name = schema_name
        self.table_name = table_name
        self.column_names = column_names

    def be_sent(self, sender: Sender) -> None:
        sender.send_size(NodeKind.SELECT.value)

        sender.send_str(self.schema_name)
        sender.send_str(self.table_name)

        sender.send_size(len(self.column_names))
        for column_name in self.column_names:
            sender.send_str(column_name)


class InsertCommand(Query):
    def __init__(
        self,
        schema_name: str,
        table_name: str,
        column_names: List[str],
        rows: List[Row],
    ) -> None:
        self.schema_name = schema_name
        self.table_name = table_name
        self.column_names = column_names
        self.rows = rows

    def be_sent(self, sender: Sender) -> None:
        sender.send_size(NodeKind.INSERT.value)

        sender.send_str(self.schema_name)
        sender.send_str(self.table_name)

        sender.send_size(len(self.column_names))
        for column_name in self.column_names:
            sender.send_str(column_name)

        sender.send_size(len(self.rows))
        for row in self.rows:
            row.be_sent(sender)
