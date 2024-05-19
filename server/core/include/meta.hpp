#pragma once

#include "server/core/include/copyable.hpp"
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

class Value : public ISerializable,
              public IStringable,
              public ICopyable<Value> {
public:
  virtual ~Value() = default;

  bool operator==(const Value &other) const;
  bool operator!=(const Value &other) const;

  virtual const Type *getType() const = 0;
  virtual bool isEqualTo(const Value *other) const = 0;
  virtual bool isLessThan(const Value *other) const = 0;
};

class IntegerValue : public Value {
  int content;

public:
  IntegerValue(int desiredContent);
  ~IntegerValue() override = default;

  const Type *getType() const override;
  std::string toString() const override;
  bool isEqualTo(const Value *other) const override;
  bool isLessThan(const Value *other) const override;

  int get() const;

  void writeTo(std::ostream &os) const override;
  std::unique_ptr<Value> copy() const override;
};

class VarcharValue : public Value {
  std::string content;

public:
  VarcharValue(std::string desiredContent);
  ~VarcharValue() override = default;

  const Type *getType() const override;
  std::string toString() const override;
  bool isEqualTo(const Value *other) const override;
  bool isLessThan(const Value *other) const override;

  const std::string &get() const;

  void writeTo(std::ostream &os) const override;
  std::unique_ptr<Value> copy() const override;
};

class Row : public ISerializable, public IStringable {
  std::vector<std::unique_ptr<Value>> values;

public:
  Row() = default;
  Row(const Row &other);
  Row(Row &&other) = default;

  ~Row() override = default;

  void append(std::unique_ptr<Value> value);
  Value *at(size_t i);

  size_t getSize() const;
  std::vector<const Type *> getTypes() const;
  const Value *at_const(size_t i) const;

  Row &operator=(const Row &other);

  bool operator<(const Row &other) const;
  bool operator==(const Row &other) const;
  bool operator!=(const Row &other) const;

  void writeTo(std::ostream &os) const override;
  std::string toString() const override;
};

struct Column : public ISerializable, public IStringable {
  std::string name;
  const Type *type;

  Column() = default;
  Column(std::string desiredName, const Type *desiredType);

  void writeTo(std::ostream &os) const override;
  std::string toString() const override;
};

class Table : public ISerializable {
  std::vector<Column> columns;
  std::vector<Row> rows;

public:
  Table(std::vector<Column> desiredColumns);
  Table(Table &&other) = default;
  Table(const Table &other) = default;
  ~Table() override = default;

  void append(Row row);
  Row *row_at(size_t i);

  size_t getRowCount() const;
  const Row *row_at_const(size_t i) const;
  const std::vector<Row> &toVector() const;
  std::vector<const Type *> getTypes() const;

  void writeTo(std::ostream &os) const override;
};
