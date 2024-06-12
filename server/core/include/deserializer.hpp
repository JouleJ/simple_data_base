#pragma once

#include <istream>
#include <memory>

#include "server/core/include/meta.hpp"
#include "server/core/include/stream.hpp"

int deserializeInteger(std::istream &is);
size_t deserializeSize(std::istream &is);
bool deserializeBoolean(std::istream &is);
std::string deserializeString(std::istream &is);

template <typename T> class BaseDeserializer : public IStream<T> {
protected:
  std::istream &is;

public:
  BaseDeserializer(std::istream &desiredIs) : is(desiredIs) {}
  ~BaseDeserializer() override = default;

  bool hasNext() override {
    is.peek();
    return is.eof() == 0;
  }
};

class TypeDeserializer : public BaseDeserializer<const Type *> {
public:
  TypeDeserializer(std::istream &desiredIs);
  ~TypeDeserializer() override = default;

  const Type *getNext() override;
};

class ColumnDeserializer : public BaseDeserializer<Column> {
public:
  ColumnDeserializer(std::istream &desiredIs);
  ~ColumnDeserializer() override = default;

  Column getNext() override;
};

class ValueDeserializer : public BaseDeserializer<std::unique_ptr<Value>> {
public:
  ValueDeserializer(std::istream &desiredIs);
  ~ValueDeserializer() override = default;

  std::unique_ptr<Value> getNext() override;
};

class RowDeserializer : public BaseDeserializer<Row> {
public:
  RowDeserializer(std::istream &desiredIs);
  ~RowDeserializer() override = default;

  Row getNext() override;
};
