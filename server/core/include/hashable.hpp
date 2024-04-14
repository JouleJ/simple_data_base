#pragma once

#include <cstddef>
#include <cstdint>

class IHashable {
public:
  virtual ~IHashable() = default;
  virtual uint64_t getHash() const = 0;
};

uint64_t computeHashOfByteArray(const uint8_t *data, size_t length);
