#pragma once

#include "server/core/include/deserializer.hpp"
#include "server/core/include/meta.hpp"
#include "server/core/include/unit.hpp"
#include "server/persistent/include/storage.hpp"

#include <algorithm>
#include <istream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>

class INode {
public:
  virtual ~INode() = default;
};

class NodeDeserializer : public BaseDeserializer<std::unique_ptr<INode>> {
public:
  NodeDeserializer(std::istream &desiredIs);
  ~NodeDeserializer() = default;

  std::unique_ptr<INode> getNext() override;
};

class IComputableNode;
class TableContext {
  std::map<std::string, size_t> columnNameToIndex;
  std::vector<Column> columns;

public:
  TableContext(std::vector<Column> desiredColumns);
  ~TableContext() = default;

  std::optional<size_t> getColumnIndex(const std::string &columnName) const;
  std::optional<Column> getNthColumn(size_t n) const;

  bool inferFilter(const IComputableNode *filterRootNode, const Row *row) const;
};

struct ComputationContext {
  const TableContext *tableContext;
  const Row *row;

  ComputationContext(const TableContext *desireedTableContext,
                     const Row *desiredRow);
};

class IComputableNode : public INode {
public:
  ~IComputableNode() = default;

  virtual std::unique_ptr<Value> compute(ComputationContext ctx) const = 0;
};

template <typename T> class Literal : public IComputableNode {
protected:
  T value;

public:
  template <typename S>
  Literal(S desiredValue) : value(std::forward<T>(desiredValue)) {}
  virtual ~Literal() = default;

  virtual std::unique_ptr<Value> makeValue() const = 0;
  std::unique_ptr<Value> compute(ComputationContext ctx) const override {
    static_cast<void>(ctx);
    return makeValue();
  }
};

class IntegerLiteral : public Literal<int> {
public:
  IntegerLiteral(int desiredValue);
  ~IntegerLiteral() = default;

  std::unique_ptr<Value> makeValue() const override;
};

class VarcharLiteral : public Literal<std::string> {
public:
  VarcharLiteral(std::string desiredValue);
  ~VarcharLiteral() = default;

  std::unique_ptr<Value> makeValue() const override;
};

template <typename T> class ICommand : public INode {
public:
  virtual ~ICommand() = default;
  virtual T execute(IStorage &storage) const = 0;
};

class Variable : public IComputableNode {
  std::string name;

public:
  Variable(std::string desiredName);
  ~Variable() override = default;

  std::unique_ptr<Value> compute(ComputationContext ctx) const override;
};

enum class BinaryOperatorKind {
  EQ,
  NEQ,
  LT,
  GT,
  LEQ,
  GEQ,
  AND,
  OR,
};

class BinaryOperator : public IComputableNode {
  BinaryOperatorKind kind;
  std::unique_ptr<IComputableNode> left, right;

public:
  BinaryOperator(BinaryOperatorKind desiredKind,
                 std::unique_ptr<IComputableNode> desiredLeft,
                 std::unique_ptr<IComputableNode> desiredRight);
  ~BinaryOperator() override = default;

  std::unique_ptr<Value> compute(ComputationContext ctx) const override;
};

class CreateTableCommand : public ICommand<Unit> {
  std::string schemaName, tableName;
  std::vector<Column> columns;

public:
  CreateTableCommand(std::string desiredSchemaName,
                     std::string desiredTableName, std::vector<Column> columns);
  ~CreateTableCommand() override = default;

  Unit execute(IStorage &storage) const override;
};

class SelectCommand : public ICommand<Table> {
  std::string schemaName, tableName;
  std::optional<std::vector<std::string>> columnNames;
  std::unique_ptr<IComputableNode> filterRootNode;

public:
  SelectCommand(std::string desiredSchemaName, std::string desiredTableName);
  SelectCommand(std::string desiredSchemaName, std::string desiredTableName,
                std::vector<std::string> desiredColumnNames);
  ~SelectCommand() override = default;

  void setFilter(std::unique_ptr<IComputableNode> desiredFilterRootNode);

  Table execute(IStorage &storage) const override;
};

class InsertCommand : public ICommand<Unit> {
  std::string schemaName, tableName;
  std::vector<std::string> columnNames;
  std::vector<Row> rows;

public:
  InsertCommand(std::string desiredSchemaName, std::string desiredTableName,
                std::vector<std::string> desiredColumnNames,
                std::vector<Row> desiredRows);
  InsertCommand(std::string desiredSchemaName, std::string desiredTableName,
                std::vector<Row> desiredRows);
  ~InsertCommand() override = default;

  Unit execute(IStorage &storage) const override;
};
