#include "server/core/include/utils.hpp"
#include "server/persistent/include/storage.hpp"
#include "server/persistent/include/utils.hpp"
#include <cstdint>
#include <ostream>

uint64_t allocateChunk(const IStorage &storage) {
  const uint64_t step = getCurrentEpoch() * 2ULL + 1ULL;

  uint64_t chunkId = 0ULL;
  while (true) {
    if (!storage.exists(chunkId)) {
      return chunkId;
    }

    chunkId += step;
  }
}

void clearChunk(IStorage &storage, uint64_t chunkId) {
  storage.write(chunkId, [](std::ostream &os) -> void {});
}
