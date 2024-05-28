#include "server/persistent/include/schema.hpp"
#include "server/core/include/deserializer.hpp"
#include "server/core/include/hashable.hpp"
#include "server/core/include/logger.hpp"
#include "server/core/include/serialize.hpp"
#include "server/core/include/stream.hpp"
#include "server/persistent/include/table.hpp"
#include "server/persistent/include/utils.hpp"

#include <cstdint>
#include <istream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>

void SchemaMetaData::writeTo(std::ostream &os) const {
  const size_t n = tables.size();
  serializeToStream(n, os);

  for (const auto &[name, chunkId] : tables) {
    serializeToStream(name, os);
    serializeHashToStream(chunkId, os);
  }
}

SchemaMetaDataDeserializer::SchemaMetaDataDeserializer(std::istream &is)
    : BaseDeserializer(is) {}

SchemaMetaData SchemaMetaDataDeserializer::getNext() {
  SchemaMetaData schmd;
  const size_t n = deserializeSize(is);
  for (size_t i = 0; i != n; ++i) {
    const std::string name = deserializeString(is);
    const uint64_t chunkId = deserializeHashFromStream(is);

    if (schmd.tables.find(name) != schmd.tables.end()) {
      getLogger().warning("Redefinition of table %q to chunk %h", name,
                          chunkId);
    }

    schmd.tables[name] = chunkId;
  }

  return schmd;
}

void initializeSchema(IStorage &storage, uint64_t metaDataChunk) {
  SchemaMetaData schmd;
  storage.writeSerializible(metaDataChunk, schmd);
}

void createTableInSchema(IStorage &storage, uint64_t metaDataChunk,
                         const std::string &tableName,
                         std::vector<Column> columns) {
  std::optional<SchemaMetaData> schmd;
  storage.read(metaDataChunk, [&](std::istream &is) -> void {
    SchemaMetaDataDeserializer schemaMetaDataDeserialzer(is);
    schmd = fetchOne(schemaMetaDataDeserialzer);
  });

  if (!schmd) {
    throw std::runtime_error(
        "Failed to read schema meta data during creation of new table");
  }

  if (schmd->tables.find(tableName) != schmd->tables.end()) {
    throw std::runtime_error("Table already exists");
  }

  const uint64_t chunkId = allocateChunk(storage);
  clearChunk(storage, chunkId);

  initializePersistentTable(storage, chunkId, std::move(columns));
  schmd->tables[tableName] = chunkId;

  storage.writeSerializible(metaDataChunk, schmd.value());
}

std::optional<uint64_t> findTableInSchema(IStorage &storage,
                                          uint64_t metaDataChunk,
                                          const std::string &tableName) {
  std::optional<SchemaMetaData> schmd;
  storage.read(metaDataChunk, [&](std::istream &is) -> void {
    SchemaMetaDataDeserializer schemaMetaDataDeserialzer(is);
    schmd = fetchOne(schemaMetaDataDeserialzer);
  });

  if (!schmd) {
    throw std::runtime_error(
        "Failed to read schema meta data during creation of new table");
  }

  const auto iter = schmd->tables.find(tableName);
  if (iter != schmd->tables.end()) {
    return iter->second;
  }

  return {};
}
