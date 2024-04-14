#include "server/core/include/meta.hpp"
#include "server/core/include/utils.hpp"

#include <string>

bool Value::operator==(const Value &other) const { return isEqualTo(&other); }

bool Value::operator!=(const Value &other) const { return !(*this == other); }

IntegerValue::IntegerValue(int desiredContent) : content(desiredContent) {}

const Type *IntegerValue::getType() const {
  return getPrimitiveTypeByName("integer");
}

std::string IntegerValue::toString() const { return std::to_string(get()); }

bool IntegerValue::isEqualTo(const Value *other) const {
  const IntegerValue *iv = dynamic_cast<const IntegerValue *>(other);
  if (iv == nullptr) {
    return false;
  }

  return get() == iv->get();
}

int IntegerValue::get() const { return content; }

VarcharValue::VarcharValue(std::string desiredContent)
    : content(std::move(desiredContent)) {}

const Type *VarcharValue::getType() const {
  return getPrimitiveTypeByName("varchar");
}

std::string VarcharValue::toString() const { return quote(get()); }

bool VarcharValue::isEqualTo(const Value *other) const {
  const VarcharValue *vv = dynamic_cast<const VarcharValue *>(other);
  if (vv == nullptr) {
    return false;
  }

  return get() == vv->get();
}

const std::string &VarcharValue::get() const { return content; }
