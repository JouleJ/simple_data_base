#include "server/core/include/hashable.hpp"
#include "server/core/include/serialize.hpp"

#include <cstddef>
#include <cstdint>
#include <sstream>

static uint64_t splitmix64(uint64_t state) {
  state += 0x9E3779B97F4A7C15;
  state = (state ^ (state >> 30ULL)) * 0xBF58476D1CE4E5B9;
  state = (state ^ (state >> 27ULL)) * 0x94D049BB133111EB;
  state ^= (state >> 31ULL);

  return state;
}

static uint64_t table[256];
static bool isTableInitialized = false;

static void initializeTable() {
  uint64_t current = 0;
  for (size_t i = 0; i != sizeof(table) / sizeof(table[0]); ++i) {
    current = splitmix64(current);
    table[i] = current;
  }

  isTableInitialized = true;
}

uint64_t computeHashOfByteArray(const uint8_t *data, size_t length) {
  if (!isTableInitialized) {
    initializeTable();
  }

  uint64_t result = 0xAB1302DD6701EF15;
  for (size_t i = 0; i != length; ++i) {
    result = splitmix64(result + table[data[i]]);
  }

  return result;
}

uint64_t ISerializable::getHash() const {
  std::stringstream buf;
  serializeToStream(*this, buf);

  const std::string s = buf.str();
  return computeHashOfByteArray(reinterpret_cast<const uint8_t *>(s.data()),
                                s.size());
}
