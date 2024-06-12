#include <stdexcept>

#include "server/core/include/meta.hpp"

static Integer integer;
static Varchar varchar;
static Boolean boolean;

const Type *getPrimitiveTypeByName(const std::string &name) {
  if (name == "integer") {
    return &integer;
  }

  if (name == "varchar") {
    return &varchar;
  }

  if (name == "boolean") {
    return &boolean;
  }

  throw std::runtime_error(std::string("No such type: ") + name);
}

bool isInteger(const Type *type) {
  return dynamic_cast<const Integer *>(type) != nullptr;
}

bool isVarchar(const Type *type) {
  return dynamic_cast<const Varchar *>(type) != nullptr;
}

bool isBoolean(const Type *type) {
  return dynamic_cast<const Boolean *>(type) != nullptr;
}

bool compareTypes(const Type *left, const Type *right) {
  if (isInteger(left)) {
    return isInteger(right);
  }

  if (isVarchar(left)) {
    return isVarchar(right);
  }

  if (isBoolean(left)) {
    return isBoolean(right);
  }

  return (left == right);
}

bool compareTypeVectors(const std::vector<const Type *> &left,
                        const std::vector<const Type *> &right) {
  const size_t n = left.size();
  if (right.size() != n) {
    return false;
  }

  for (size_t i = 0; i < n; ++i) {
    if (!compareTypes(left[i], right[i])) {
      return false;
    }
  }

  return true;
}
