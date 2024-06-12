#include "server/query/include/ast.hpp"
#include "server/core/include/meta.hpp"
#include "server/core/include/stream.hpp"
#include "server/core/include/unit.hpp"
#include "server/persistent/include/storage.hpp"

#include <memory>
#include <optional>

TableContext::TableContext(std::vector<Column> desiredColumns)
    : columns(std::move(desiredColumns)) {
  VectorStream<Column>(columns.begin(), columns.end())
      .forEach([&](size_t i, const Column &column) {
        columnNameToIndex[column.name] = i;
      });
}

std::optional<size_t>
TableContext::getColumnIndex(const std::string &columnName) const {
  const auto iter = columnNameToIndex.find(columnName);
  if (iter == columnNameToIndex.end()) {
    return {};
  }

  return iter->second;
}

std::optional<Column> TableContext::getNthColumn(size_t n) const {
  if (n >= columns.size()) {
    return {};
  }

  return columns[n];
}

bool TableContext::inferFilter(const IComputableNode *filterRootNode,
                               const Row *row) const {
  return filterRootNode->compute(ComputationContext(this, row))->test();
}

ComputationContext::ComputationContext(const TableContext *desiredTableContext,
                                       const Row *desiredRow)
    : tableContext(desiredTableContext), row(desiredRow) {}

IntegerLiteral::IntegerLiteral(int desiredValue) : Literal(desiredValue) {}

std::unique_ptr<Value> IntegerLiteral::makeValue() const {
  return std::make_unique<IntegerValue>(value);
}

VarcharLiteral::VarcharLiteral(std::string desiredValue)
    : Literal(std::move(desiredValue)) {}

std::unique_ptr<Value> VarcharLiteral::makeValue() const {
  return std::make_unique<VarcharValue>(value);
}

Variable::Variable(std::string desiredName) : name(std::move(desiredName)) {}

BinaryOperator::BinaryOperator(BinaryOperatorKind desiredKind,
                               std::unique_ptr<IComputableNode> desiredLeft,
                               std::unique_ptr<IComputableNode> desiredRight)
    : kind(desiredKind), left(std::move(desiredLeft)),
      right(std::move(desiredRight)) {}

CreateTableCommand::CreateTableCommand(std::string desiredSchemaName,
                                       std::string desiredTableName,
                                       std::vector<Column> desiredColumns)
    : schemaName(std::move(desiredSchemaName)),
      tableName(std::move(desiredTableName)),
      columns(std::move(desiredColumns)) {}

SelectCommand::SelectCommand(std::string desiredSchemaName,
                             std::string desiredTableName)
    : schemaName(std::move(desiredSchemaName)),
      tableName(std::move(desiredTableName)) {}

SelectCommand::SelectCommand(std::string desiredSchemaName,
                             std::string desiredTableName,
                             std::vector<std::string> desiredColumnNames)
    : schemaName(std::move(desiredSchemaName)),
      tableName(std::move(desiredTableName)),
      columnNames(std::move(desiredColumnNames)) {}

void SelectCommand::setFilter(
    std::unique_ptr<IComputableNode> desiredFilterRootNode) {
  filterRootNode = std::move(desiredFilterRootNode);
}

InsertCommand::InsertCommand(std::string desiredSchemaName,
                             std::string desiredTableName,
                             std::vector<std::string> desiredColumnNames,
                             std::vector<Row> desiredRows)
    : schemaName(std::move(desiredSchemaName)),
      tableName(std::move(desiredTableName)),
      columnNames(std::move(desiredColumnNames)), rows(std::move(desiredRows)) {
}

InsertCommand::InsertCommand(std::string desiredSchemaName,
                             std::string desiredTableName,
                             std::vector<Row> desiredRows)
    : schemaName(std::move(desiredSchemaName)),
      tableName(std::move(desiredTableName)), rows(std::move(desiredRows)) {}
