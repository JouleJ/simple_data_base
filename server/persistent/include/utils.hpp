#pragma once

#include "server/persistent/include/storage.hpp"

#include <cstdint>

uint64_t allocateChunk(const IStorage &storage);
void clearChunk(IStorage &storage, uint64_t chunkId);
