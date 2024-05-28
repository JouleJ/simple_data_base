#include "server/query/include/ast.hpp"
#include "server/core/include/meta.hpp"
#include "server/core/include/unit.hpp"
#include "server/persistent/include/storage.hpp"

#include <memory>

IntegerLiteral::IntegerLiteral(int desiredValue) : Literal(desiredValue) {}

std::unique_ptr<Value> IntegerLiteral::makeValue() const {
  return std::make_unique<IntegerValue>(value);
}

VarcharLiteral::VarcharLiteral(std::string desiredValue)
    : Literal(std::move(desiredValue)) {}

std::unique_ptr<Value> VarcharLiteral::makeValue() const {
  return std::make_unique<VarcharValue>(value);
}

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
