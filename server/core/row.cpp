#include "server/core/include/meta.hpp"
#include "server/core/include/serialize.hpp"

#include <iostream>
#include <sstream>

Row::Row(const Row &other) {
  const size_t n = other.getSize();
  values.reserve(n);

  for (size_t i = 0U; i != n; ++i) {
    values.push_back(other.at_const(i)->copy());
  }
}

void Row::append(std::unique_ptr<Value> value) {
  values.push_back(std::move(value));
}

Value *Row::at(size_t idx) { return values.at(idx).get(); }

size_t Row::getSize() const { return values.size(); }

std::vector<const Type *> Row::getTypes() const {
  std::vector<const Type *> types;
  types.reserve(values.size());

  for (const auto &value : values) {
    types.push_back(value->getType());
  }

  return types;
}

const Value *Row::at_const(size_t idx) const { return values.at(idx).get(); }

std::string Row::toString() const {
  std::stringstream buf;
  buf << "Row[";

  const size_t size = getSize();
  if (size != 0) {
    buf << at_const(0)->toString();
    for (size_t i = 1; i != size; ++i) {
      buf << ", " << at_const(i)->toString();
    }
  }

  buf << "]";
  return buf.str();
}

Row &Row::operator=(const Row &other) {
  const size_t n = other.getSize();
  values.clear();
  values.reserve(n);

  for (size_t i = 0U; i != n; ++i) {
    values.push_back(other.at_const(i)->copy());
  }

  return *this;
}

bool Row::operator==(const Row &other) const {
  const size_t size = getSize();
  if (other.getSize() != size) {
    return false;
  }

  for (size_t i = 0; i != size; ++i) {
    const Value *element = at_const(i);
    const Value *otherElement = other.at_const(i);

    if (!element->isEqualTo(otherElement)) {
      return false;
    }
  }

  return true;
}

bool Row::operator!=(const Row &other) const { return !(*this == other); }

bool Row::operator<(const Row &other) const {
  const size_t n = std::min<size_t>(getSize(), other.getSize());
  for (size_t i = 0; i != n; ++i) {
    if (at_const(i)->isLessThan(other.at_const(i))) {
      return true;
    }

    if (other.at_const(i)->isLessThan(at_const(i))) {
      return false;
    }
  }

  return getSize() < other.getSize();
}
