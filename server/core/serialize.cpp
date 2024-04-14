#include "server/core/include/serialize.hpp"
#include "server/core/include/meta.hpp"

void serializeToStream(int i, std::ostream &os) {
  unsigned char buf[sizeof(i) * 2], lastTok;
  size_t head = 0;

  if (i > 0) {
    lastTok = static_cast<unsigned char>(Sign::PLUS);
  } else if (i < 0) {
    lastTok = static_cast<unsigned char>(Sign::MINUS);
    i = -i;
  } else {
    lastTok = static_cast<unsigned char>(Sign::ZERO);
  }

  while (i > 0) {
    unsigned char tok = static_cast<unsigned char>(i & 0x7F);
    buf[head++] = tok;

    i >>= 7;
  }

  buf[head++] = lastTok;
  os.write(reinterpret_cast<char *>(&buf[0]), head);
}

void serializeToStream(size_t size, std::ostream &os) {
  unsigned char buf[sizeof(size) * 2];
  size_t head = 0;

  while (size != 0) {
    unsigned char tok =
        static_cast<unsigned char>(size & static_cast<size_t>(0x7F));
    buf[head++] = tok;

    size >>= 7U;
  }

  if (head == 0) {
    buf[head++] = 0;
  }

  buf[head - 1] |= static_cast<unsigned char>(0x80);
  os.write(reinterpret_cast<char *>(&buf[0]), head);
}

void serializeToStream(const std::string &s, std::ostream &os) {
  const size_t size = s.size();
  serializeToStream(size, os);

  os.write(s.data(), size);
}

void serializeToStream(const ISerializable &object, std::ostream &os) {
  object.writeTo(os);
}

void Integer::writeTo(std::ostream &os) const {
  const size_t typeId = static_cast<size_t>(TypeKind::INTEGER);
  serializeToStream(typeId, os);
}

void Varchar::writeTo(std::ostream &os) const {
  const size_t typeId = static_cast<size_t>(TypeKind::VARCHAR);
  serializeToStream(typeId, os);
}

void IntegerValue::writeTo(std::ostream &os) const {
  const Type &type = *getType();
  serializeToStream(type, os);

  const auto value = get();
  serializeToStream(value, os);
}

void VarcharValue::writeTo(std::ostream &os) const {
  const Type &type = *getType();
  serializeToStream(type, os);

  const auto value = get();
  serializeToStream(value, os);
}

void Row::writeTo(std::ostream &os) const {
  const size_t size = getSize();
  serializeToStream(size, os);

  for (size_t i = 0; i != size; ++i) {
    const Value &value = *at_const(i);
    serializeToStream(value, os);
  }
}

void Table::writeTo(std::ostream &os) const {
  const size_t size = getRowCount();
  serializeToStream(size, os);

  for (size_t i = 0; i != size; ++i) {
    const Row &row = *row_at_const(i);
    serializeToStream(row, os);
  }
}
