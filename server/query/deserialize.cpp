#include "server/core/include/deserializer.hpp"
#include "server/query/include/ast.hpp"

#include <istream>
#include <memory>
#include <stdexcept>
#include <string>

enum class NodeKind : size_t {
  CREATE_TABLE = 1,
  SELECT = 2,
  INSERT = 3,
};

static std::unique_ptr<INode> parseCreateTableCommand(std::istream &is) {
  auto schemaName = deserializeString(is);
  auto tableName = deserializeString(is);

  const size_t columnCount = deserializeSize(is);
  std::vector<Column> columns;

  ColumnDeserializer columnDeserializer(is);
  for (size_t i = 0; i != columnCount; ++i) {
    columns.push_back(columnDeserializer.getNext());
  }

  return std::make_unique<CreateTableCommand>(
      std::move(schemaName), std::move(tableName), std::move(columns));
}

static std::unique_ptr<INode> parseSelectCommand(std::istream &is) {
  auto schemaName = deserializeString(is);
  auto tableName = deserializeString(is);

  const size_t columnNameCount = deserializeSize(is);
  std::vector<std::string> columnNames;

  for (size_t i = 0; i != columnNameCount; ++i) {
    columnNames.push_back(deserializeString(is));
  }

  return std::make_unique<SelectCommand>(
      std::move(schemaName), std::move(tableName), std::move(columnNames));
}

static std::unique_ptr<INode> parseInsertCommand(std::istream &is) {
  auto schemaName = deserializeString(is);
  auto tableName = deserializeString(is);

  const size_t columnNameCount = deserializeSize(is);
  std::vector<std::string> columnNames;

  for (size_t i = 0; i != columnNameCount; ++i) {
    columnNames.push_back(deserializeString(is));
  }

  std::vector<Row> rows;

  const size_t rowCount = deserializeSize(is);
  RowDeserializer rowDeserializer(is);
  for (size_t i = 0; i != rowCount; ++i) {
    rows.push_back(rowDeserializer.getNext());
  }

  if (columnNames.empty()) {
    return std::make_unique<InsertCommand>(
        std::move(schemaName), std::move(tableName), std::move(rows));
  } else {
    return std::make_unique<InsertCommand>(
        std::move(schemaName), std::move(tableName), std::move(columnNames),
        std::move(rows));
  }
}

static std::unique_ptr<INode> parseNode(std::istream &is) {
  const auto nodeKind = static_cast<NodeKind>(deserializeSize(is));
  switch (nodeKind) {
    case NodeKind::CREATE_TABLE:
      return parseCreateTableCommand(is);

    case NodeKind::SELECT:
      return parseSelectCommand(is);

    case NodeKind::INSERT:
      return parseInsertCommand(is);
  }

  throw std::runtime_error("Unknown node kind in parseNode");
}

NodeDeserializer::NodeDeserializer(std::istream &desiredIs)
    : BaseDeserializer(desiredIs) {}

std::unique_ptr<INode> NodeDeserializer::getNext() { return parseNode(is); }
