#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>

class IHashable {
public:
  virtual ~IHashable() = default;
  virtual uint64_t getHash() const = 0;
};

uint64_t computeHashOfByteArray(const uint8_t *data, size_t length);

void serializeHashToStream(uint64_t hash, std::ostream &os);
uint64_t deserializeHashFromStream(std::istream &is);
