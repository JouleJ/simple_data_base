#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>

enum class TypeKind : size_t {
  INTEGER = 1,
  VARCHAR = 2,
};

enum class Sign : uint8_t {
  PLUS = 0x81,
  MINUS = 0x82,
  ZERO = 0x83,
};

class ISerializable {
public:
  virtual ~ISerializable() = default;

  virtual void writeTo(std::ostream &os) const = 0;
};

void serializeToStream(int i, std::ostream &os);
void serializeToStream(size_t sz, std::ostream &os);
void serializeToStream(const std::string &s, std::ostream &os);
void serializeToStream(const ISerializable &object, std::ostream &os);
