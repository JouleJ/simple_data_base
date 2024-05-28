#include "server/persistent/include/database.hpp"
#include "server/core/include/deserializer.hpp"
#include "server/core/include/logger.hpp"
#include "server/core/include/serialize.hpp"
#include "server/core/include/stream.hpp"
#include "server/persistent/include/schema.hpp"
#include "server/persistent/include/storage.hpp"
#include "server/persistent/include/utils.hpp"

#include <cstdint>
#include <istream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>

void DataBaseMetaData::writeTo(std::ostream &os) const {
  const size_t n = schemas.size();
  serializeToStream(n, os);

  for (const auto &[name, chunkId] : schemas) {
    serializeToStream(name, os);
    serializeHashToStream(chunkId, os);
  }
}

DataBaseMetaDataDeserializer::DataBaseMetaDataDeserializer(std::istream &is)
    : BaseDeserializer(is) {}

DataBaseMetaData DataBaseMetaDataDeserializer::getNext() {
  DataBaseMetaData dbmd;
  const size_t n = deserializeSize(is);
  for (size_t i = 0; i != n; ++i) {
    const std::string name = deserializeString(is);
    const uint64_t chunkId = deserializeHashFromStream(is);

    if (dbmd.schemas.find(name) != dbmd.schemas.end()) {
      getLogger().warning(
          "Redefinition of schema %q while parsing database specification",
          name);
    }

    dbmd.schemas[name] = chunkId;
  }

  return dbmd;
}

void initializeDataBase(IStorage &storage) {
  DataBaseMetaData dbmd;
  storage.writeSerializible(dataBaseRootChunk, dbmd);
}

uint64_t createSchemaInDataBase(IStorage &storage, std::string name) {
  std::optional<DataBaseMetaData> dbmd;
  storage.read(dataBaseRootChunk, [&](std::istream &is) -> void {
    DataBaseMetaDataDeserializer dbmdDeserializer(is);
    dbmd = fetchOne(dbmdDeserializer);
  });

  if (!dbmd) {
    throw std::runtime_error("Failed to read data base specification while "
                             "attempting to create schema in data base");
  }

  if (dbmd->schemas.find(name) != dbmd->schemas.end()) {
    throw std::runtime_error("Schema already exists");
  }

  const uint64_t chunkId = allocateChunk(storage);
  clearChunk(storage, chunkId);

  dbmd->schemas[name] = chunkId;
  storage.writeSerializible(dataBaseRootChunk, dbmd.value());

  initializeSchema(storage, chunkId);
  return chunkId;
}

std::optional<uint64_t> findSchemaInDataBase(IStorage &storage,
                                             const std::string &name) {
  std::optional<DataBaseMetaData> dbmd;
  storage.read(dataBaseRootChunk, [&](std::istream &is) -> void {
    DataBaseMetaDataDeserializer dbmdDeserializer(is);
    dbmd = fetchOne(dbmdDeserializer);
  });

  if (!dbmd) {
    throw std::runtime_error("Failed to read data base specification while "
                             "attempting to find schema by name");
  }

  const auto iter = dbmd->schemas.find(name);
  if (iter != dbmd->schemas.end()) {
    return iter->second;
  }

  return {};
}
