#include "server/core/include/deserializer.hpp"
#include "server/core/include/hashable.hpp"
#include "server/core/include/meta.hpp"
#include "server/core/include/serialize.hpp"

#include <istream>
#include <stdexcept>
#include <string>

int deserializeInteger(std::istream &is) {
  unsigned char buf[sizeof(int) * 2];
  size_t head = 0;

  while (!is.eof()) {
    is.read(reinterpret_cast<char *>(&buf[head]), sizeof(buf[head]));
    if (is.gcount() != sizeof(buf[head])) {
      break;
    }

    ++head;
    if ((buf[head - 1] & static_cast<unsigned char>(0x80)) != 0U) {
      break;
    }
  }

  if (head == 0) {
    throw std::runtime_error("Unexpected EOF on parsing integer");
  }

  int result = 0;
  for (size_t i = 0; i != head - 1U; ++i) {
    result |= static_cast<int>(buf[i]) << static_cast<int>(i * 7U);
  }

  const Sign lastTok = static_cast<Sign>(buf[head - 1]);
  switch (lastTok) {
    case Sign::PLUS:
      return result;

    case Sign::MINUS:
      return -result;

    case Sign::ZERO:
      return 0;
  }

  throw std::runtime_error("Invalaid sign: " +
                           std::to_string(static_cast<int>(buf[head - 1])));
}

size_t deserializeSize(std::istream &is) {
  unsigned char buf[sizeof(size_t) * 2];
  size_t head = 0;

  bool stop = false;
  while (!is.eof() && !stop) {
    is.read(reinterpret_cast<char *>(&buf[head]), sizeof(buf[head]));
    if (is.gcount() != sizeof(buf[head])) {
      break;
    }

    ++head;
    stop = ((buf[head - 1] & static_cast<unsigned char>(0x80)) != 0U);
  }

  if (head == 0 || !stop) {
    throw std::runtime_error("Unexpected EOF on parsing size_t");
  }

  buf[head - 1] &= ~static_cast<unsigned char>(0x80);

  size_t result = 0;
  for (size_t i = 0; i != head; ++i) {
    result |= static_cast<size_t>(buf[i]) << (i * 7U);
  }

  return result;
}

std::string deserializeString(std::istream &is) {
  const size_t size = deserializeSize(is);
  std::string result(size, '\0');
  is.read(result.data(), size);
  if (is.gcount() != size) {
    throw std::runtime_error("Failed to read string: unexpected EOF");
  }

  return result;
}

bool deserializeBoolean(std::istream &is) {
  unsigned char chr;
  is.read(reinterpret_cast<char *>(&chr), 1);

  if (chr == 0U) {
    return false;
  } else if (chr == static_cast<unsigned char>(0xFF)) {
    return true;
  }

  throw std::runtime_error(std::string("Invalid boolean-encoding byte: ") +
                           std::to_string(chr));
}

TypeDeserializer::TypeDeserializer(std::istream &desiredIs)
    : BaseDeserializer(desiredIs) {}

const Type *TypeDeserializer::getNext() {
  const size_t typeKindId = deserializeSize(is);
  const TypeKind typeKind = static_cast<TypeKind>(typeKindId);

  switch (typeKind) {
    case TypeKind::INTEGER:
      return getPrimitiveTypeByName("integer");

    case TypeKind::VARCHAR:
      return getPrimitiveTypeByName("varchar");

    case TypeKind::BOOLEAN:
      return getPrimitiveTypeByName("boolean");

    default:
      break;
  }

  throw std::runtime_error("Unknown typeKindId: " + std::to_string(typeKindId));
}

ColumnDeserializer::ColumnDeserializer(std::istream &desiredIs)
    : BaseDeserializer(desiredIs) {}

Column ColumnDeserializer::getNext() {
  Column column;
  column.name = deserializeString(is);

  TypeDeserializer typeDeserializer(is);
  if (!typeDeserializer.hasNext()) {
    throw std::runtime_error("Unexpected EOF when trying to parse column type");
  }

  column.type = typeDeserializer.getNext();
  return column;
}

ValueDeserializer::ValueDeserializer(std::istream &desiredIs)
    : BaseDeserializer(desiredIs) {}

std::unique_ptr<Value> ValueDeserializer::getNext() {
  const Type *type = TypeDeserializer(is).getNext();

  if (isInteger(type)) {
    const int content = deserializeInteger(is);
    return std::make_unique<IntegerValue>(content);
  }

  if (isVarchar(type)) {
    const std::string content = deserializeString(is);
    return std::make_unique<VarcharValue>(content);
  }

  if (isBoolean(type)) {
    const bool content = deserializeBoolean(is);
    return std::make_unique<BooleanValue>(content);
  }

  throw std::runtime_error("Unknown type: cannot deserialize value");
}

RowDeserializer::RowDeserializer(std::istream &desiredIs)
    : BaseDeserializer(desiredIs) {}

Row RowDeserializer::getNext() {
  const size_t rowSize = deserializeSize(is);
  ValueDeserializer vd(is);

  Row row;
  for (size_t i = 0; i != rowSize; ++i) {
    row.append(vd.getNext());
  }

  return row;
}

uint64_t deserializeHashFromStream(std::istream &is) {
  uint64_t result = 0ULL;
  unsigned char buf[8];

  is.read(reinterpret_cast<char *>(&buf[0]), sizeof(buf));
  if (is.gcount() != sizeof(buf)) {
    throw std::runtime_error("Failed to read hash: unexpected EOF");
  }

  for (uint64_t i = 0; i < 8; ++i) {
    result |= (static_cast<uint64_t>(buf[i]) << (8ULL * i));
  }

  return result;
}
