#pragma once

#include "server/core/include/serialize.hpp"
#include "server/core/include/stream.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <istream>
#include <memory>
#include <optional>

class IStorage {
public:
  virtual ~IStorage() = default;

  virtual bool exists(uint64_t chunkId) const = 0;

  virtual bool read(uint64_t chunkId,
                    std::function<void(std::istream &)> action) const = 0;
  virtual void write(uint64_t chunkId,
                     std::function<void(std::ostream &)> action) = 0;
  virtual void append(uint64_t chunkId,
                      std::function<void(std::ostream &)> action) = 0;
  virtual void remove(uint64_t chunkId) = 0;

  void writeSerializible(uint64_t chunkId, const ISerializable &object);
  void appendSerializible(uint64_t chunkId, const ISerializable &object);
};

template <typename Deserializer, typename T>
std::optional<T> deserializeOne(const IStorage &storage, uint64_t chunkId) {
  std::optional<T> result;
  storage.read(chunkId, [&](std::istream &is) -> void {
    Deserializer deserializer(is);
    result = std::move(fetchOne(deserializer));
  });

  return result;
}

std::unique_ptr<IStorage> makeMemoryStorage();
std::unique_ptr<IStorage> makeDiskStorage(const std::filesystem::path &root);
