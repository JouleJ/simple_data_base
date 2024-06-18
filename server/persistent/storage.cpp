#include "server/persistent/include/storage.hpp"
#include "server/core/include/serialize.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

void IStorage::writeSerializible(uint64_t chunkId,
                                 const ISerializable &object) {
  write(chunkId, [&](std::ostream &os) -> void { object.writeTo(os); });
}

void IStorage::appendSerializible(uint64_t chunkId,
                                  const ISerializable &object) {
  append(chunkId, [&](std::ostream &os) -> void { object.writeTo(os); });
}

class MemoryStorage : public IStorage {
  std::map<uint64_t, std::string> data;

public:
  MemoryStorage() = default;
  ~MemoryStorage() override = default;

  bool exists(uint64_t chunkId) const override {
    return data.find(chunkId) != data.end();
  }

  bool read(uint64_t chunkId,
            std::function<void(std::istream &is)> action) const override {
    const auto iter = data.find(chunkId);
    if (iter == data.end()) {
      return false;
    }

    std::stringstream ss(iter->second);
    action(ss);
    return true;
  }

  void write(uint64_t chunkId,
             std::function<void(std::ostream &os)> action) override {
    std::stringstream ss;
    action(ss);

    data[chunkId] = ss.str();
  }

  void append(uint64_t chunkId,
              std::function<void(std::ostream &os)> action) override {
    std::stringstream ss;
    action(ss);

    data[chunkId] += ss.str();
  }

  void remove(uint64_t chunkId) override {
    const auto iter = data.find(chunkId);
    if (iter != data.end()) {
      data.erase(iter);
    } else {
      std::cerr << "Cannot remove chunk from memory storage twice: " << chunkId
                << "\n";
    }
  }
};

std::unique_ptr<IStorage> makeMemoryStorage() {
  return std::make_unique<MemoryStorage>();
}

class DiskStorage : public IStorage {
  std::filesystem::path root;

  std::filesystem::path getChunkPath(uint64_t chunkId) const {
    const std::string chunkName = std::to_string(chunkId) + std::string(".sdb");
    return root / chunkName;
  }

public:
  DiskStorage(const std::filesystem::path &desiredRoot) : root(desiredRoot) {}
  ~DiskStorage() override = default;

  bool exists(uint64_t chunkId) const override {
    return std::filesystem::exists(getChunkPath(chunkId));
  }

  bool read(uint64_t chunkId,
            std::function<void(std::istream &is)> action) const override {
    std::ifstream ifs(getChunkPath(chunkId),
                      std::fstream::in | std::fstream::binary);
    if (!ifs.is_open()) {
      return false;
    }

    action(ifs);
    ifs.close();
    return true;
  }

  void write(uint64_t chunkId,
             std::function<void(std::ostream &os)> action) override {
    std::ofstream ofs(getChunkPath(chunkId), std::fstream::out |
                                                 std::fstream::binary |
                                                 std::fstream::trunc);
    if (!ofs.is_open()) {
      throw std::runtime_error(
          std::string("[DiskStrorage::write] Failed to open file for chunk ") +
          std::to_string(chunkId));
    }

    action(ofs);
    ofs.close();
  }

  void append(uint64_t chunkId,
              std::function<void(std::ostream &os)> action) override {
    std::ofstream ofs(getChunkPath(chunkId), std::fstream::out |
                                                 std::fstream::binary |
                                                 std::fstream::app);
    if (!ofs.is_open()) {
      throw std::runtime_error(
          std::string("[DiskStrorage::append] Failed to open file for chunk ") +
          std::to_string(chunkId));
    }

    action(ofs);
    ofs.close();
  }

  void remove(uint64_t chunkId) override {
    const auto chunkPath = getChunkPath(chunkId);
    if (std::filesystem::exists(chunkPath)) {
      std::filesystem::remove(chunkPath);
    } else {
      std::cerr << "Cannot remove chunk from disk storage twice: " << chunkId
                << "\n";
    }
  }
};

std::unique_ptr<IStorage> makeDiskStorage(const std::filesystem::path &root) {
  return std::make_unique<DiskStorage>(root);
}
