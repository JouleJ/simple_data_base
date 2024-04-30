#pragma once

#include "server/core/include/serialize.hpp"
#include "server/core/include/stringable.hpp"

#include <memory>
#include <string>
#include <vector>

class Type : public ISerializable {
public:
  virtual ~Type() = default;
  virtual std::string getName() const = 0;
};

class Integer : public Type {
public:
  ~Integer() override = default;
  std::string getName() const override { return "integer"; }

  void writeTo(std::ostream &os) const override;
};

class Varchar : public Type {
public:
  ~Varchar() override = default;
  std::string getName() const override { return "varchar"; }

  void writeTo(std::ostream &os) const override;
};

const Type *getPrimitiveTypeByName(const std::string &name);

bool isInteger(const Type *type);
bool isVarchar(const Type *type);

bool compareTypes(const Type *left, const Type *right);
bool compareTypeVectors(const std::vector<const Type *> &left,
                        const std::vector<const Type *> &right);

class Value : public ISerializable, public IStringable {
public:
  virtual ~Value() = default;

  bool operator==(const Value &other) const;
  bool operator!=(const Value &other) const;

  virtual const Type *getType() const = 0;
  virtual bool isEqualTo(const Value *other) const = 0;
};

class IntegerValue : public Value {
  int content;

public:
  IntegerValue(int desiredContent);
  ~IntegerValue() override = default;

  const Type *getType() const override;
  std::string toString() const override;
  bool isEqualTo(const Value *other) const override;

  int get() const;

  void writeTo(std::ostream &os) const override;
};

class VarcharValue : public Value {
  std::string content;

public:
  VarcharValue(std::string desiredContent);
  ~VarcharValue() override = default;

  const Type *getType() const override;
  std::string toString() const override;
  bool isEqualTo(const Value *other) const override;

  const std::string &get() const;

  void writeTo(std::ostream &os) const override;
};

class Row : public ISerializable, public IStringable {
  std::vector<std::unique_ptr<Value>> values;

public:
  Row() = default;
  Row(Row &&other) = default;
  ;
  ~Row() override = default;

  void append(std::unique_ptr<Value> value);
  Value *at(size_t i);

  size_t getSize() const;
  std::vector<const Type *> getTypes() const;
  const Value *at_const(size_t i) const;

  bool operator==(const Row &other) const;
  bool operator!=(const Row &other) const;

  void writeTo(std::ostream &os) const override;
  std::string toString() const override;
};

class Table : public ISerializable {
  std::vector<const Type *> columnTypes;
  std::vector<Row> rows;

public:
  Table(std::vector<const Type *> desiredColumnTypes);
  Table(Table &&other) = default;
  Table(const Table &other) = delete;
  ~Table() override = default;

  void append(Row row);
  Row *row_at(size_t i);

  size_t getRowCount() const;
  const Row *row_at_const(size_t i) const;

  void writeTo(std::ostream &os) const override;
};
