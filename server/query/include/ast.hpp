#pragma once

#include "server/core/include/meta.hpp"
#include "server/core/include/unit.hpp"
#include "server/persistent/include/storage.hpp"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <utility>

class INode {
public:
  virtual ~INode() = default;
};

template <typename T> class Literal : public INode {
protected:
  T value;

public:
  template <typename S>
  Literal(S desiredValue) : value(std::forward<T>(desiredValue)) {}
  virtual ~Literal() = default;

  virtual std::unique_ptr<Value> makeValue() const = 0;
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
  /* TODO: Add filter node here */

public:
  SelectCommand(std::string desiredSchemaName, std::string desiredTableName);
  SelectCommand(std::string desiredSchemaName, std::string desiredTableName,
                std::vector<std::string> desiredColumnNames);
  ~SelectCommand() override = default;

  Table execute(IStorage &storage) const override;
};
