#include "server/test/include/context.hpp"

static std::unique_ptr<IStorage> memoryStorage;

IStorage &WithMemoryStorage::getStorage() {
  if (!memoryStorage) {
    memoryStorage = makeMemoryStorage();
  }

  return *memoryStorage;
}

const IStorage &WithMemoryStorage::getStorageConst() const {
  if (!memoryStorage) {
    memoryStorage = makeMemoryStorage();
  }

  return *memoryStorage;
}
