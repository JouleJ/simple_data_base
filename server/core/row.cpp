#include "server/core/include/meta.hpp"
#include "server/core/include/serialize.hpp"

#include <sstream>

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

bool Row::operator==(const Row &other) const {
  const size_t size = getSize();
  if (other.getSize() != size) {
    return false;
  }

  for (size_t i = 0; i != size; ++i) {
    const Value *element = at_const(i);
    const Value *otherElement = at_const(i);

    if (!element->isEqualTo(otherElement)) {
      return false;
    }
  }

  return true;
}

bool Row::operator!=(const Row &other) const { return !(*this == other); }
