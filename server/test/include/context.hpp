#pragma once

#include "server/persistent/include/storage.hpp"

class WithMemoryStorage {
public:
  IStorage &getStorage();
  const IStorage &getStorageConst() const;
};
