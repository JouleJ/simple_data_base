#include "server/core/include/logger.hpp"
#include "server/core/include/meta.hpp"
#include "server/core/include/utils.hpp"

#include <memory>
#include <string>

bool Value::operator==(const Value &other) const { return isEqualTo(&other); }

bool Value::operator!=(const Value &other) const { return !(*this == other); }

bool Value::operator<(const Value &other) const { return isLessThan(&other); }

bool Value::operator>(const Value &other) const { return (other < *this); }

bool Value::operator<=(const Value &other) const { return !(*this > other); }

bool Value::operator>=(const Value &other) const { return !(*this < other); }

IntegerValue::IntegerValue(int desiredContent) : content(desiredContent) {}

const Type *IntegerValue::getType() const {
  return getPrimitiveTypeByName("integer");
}

std::string IntegerValue::toString() const { return std::to_string(get()); }

bool IntegerValue::isEqualTo(const Value *other) const {
  if (other == nullptr) {
    getLogger().warning("Calling IntegerValue::isEqualTo(nullptr)");
    return false;
  }

  const IntegerValue *iv = dynamic_cast<const IntegerValue *>(other);
  if (iv == nullptr) {
    return false;
  }

  return get() == iv->get();
}

bool IntegerValue::isLessThan(const Value *other) const {
  if (other == nullptr) {
    getLogger().warning("Calling IntegerValue::isLessThan(nullptr)");
    return false;
  }

  const IntegerValue *iv = dynamic_cast<const IntegerValue *>(other);
  if (iv == nullptr) {
    return getType()->getName() < other->getType()->getName();
  }

  return get() < iv->get();
}

bool IntegerValue::test() const { return (content != 0); }

int IntegerValue::get() const { return content; }

std::unique_ptr<Value> IntegerValue::copy() const {
  return std::make_unique<IntegerValue>(get());
}

VarcharValue::VarcharValue(std::string desiredContent)
    : content(std::move(desiredContent)) {}

const Type *VarcharValue::getType() const {
  return getPrimitiveTypeByName("varchar");
}

std::string VarcharValue::toString() const { return quote(get()); }

bool VarcharValue::isEqualTo(const Value *other) const {
  if (other == nullptr) {
    getLogger().warning("Calling VarcharValue::isEqualTo(nullptr)");
    return false;
  }

  const VarcharValue *vv = dynamic_cast<const VarcharValue *>(other);
  if (vv == nullptr) {
    return false;
  }

  return get() == vv->get();
}

bool VarcharValue::isLessThan(const Value *other) const {
  if (other == nullptr) {
    getLogger().warning("Calling VarcharValue::isLessThan(nullptr)");
    return false;
  }

  const VarcharValue *vv = dynamic_cast<const VarcharValue *>(other);
  if (vv == nullptr) {
    return getType()->getName() < other->getType()->getName();
  }

  return get() < vv->get();
}

bool VarcharValue::test() const { return !content.empty(); }

const std::string &VarcharValue::get() const { return content; }

std::unique_ptr<Value> VarcharValue::copy() const {
  return std::make_unique<VarcharValue>(get());
}

BooleanValue::BooleanValue(bool desiredContent) : content(desiredContent) {}

const Type *BooleanValue::getType() const {
  return getPrimitiveTypeByName("boolean");
}

std::string BooleanValue::toString() const {
  if (content) {
    return "TRUE";
  } else {
    return "FALSE";
  }
}

bool BooleanValue::isEqualTo(const Value *other) const {
  if (other == nullptr) {
    getLogger().warning("Calling BooleanValue::isEqualTo(nullptr)");
    return false;
  }

  const BooleanValue *bv = dynamic_cast<const BooleanValue *>(other);
  if (bv == nullptr) {
    return false;
  }

  return get() == bv->get();
}

bool BooleanValue::isLessThan(const Value *other) const {
  if (other == nullptr) {
    getLogger().warning("Calling BooleanValue::isLessThan(nullptr)");
    return false;
  }

  const BooleanValue *bv = dynamic_cast<const BooleanValue *>(other);
  if (bv == nullptr) {
    return getType()->getName() < other->getType()->getName();
  }

  return static_cast<int>(get()) < static_cast<int>(bv->get());
}

bool BooleanValue::test() const { return get(); }

bool BooleanValue::get() const { return content; }

std::unique_ptr<Value> BooleanValue::copy() const {
  return std::make_unique<BooleanValue>(get());
}

std::unique_ptr<Value> computeAnd(std::unique_ptr<Value> left,
                                  std::unique_ptr<Value> right) {
  if (left->test()) {
    return right;
  }

  return left;
}

std::unique_ptr<Value> computeOr(std::unique_ptr<Value> left,
                                 std::unique_ptr<Value> right) {
  if (left->test()) {
    return left;
  }

  return right;
}
