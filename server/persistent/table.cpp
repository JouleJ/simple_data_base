#include "server/persistent/include/table.hpp"
#include "server/core/include/deserializer.hpp"
#include "server/core/include/serialize.hpp"
#include "server/core/include/stream.hpp"
#include "server/persistent/include/storage.hpp"
#include "server/persistent/include/utils.hpp"

#include <iostream>
#include <istream>
#include <optional>
#include <ostream>
#include <stdexcept>

void PersistentTableMetaData::writeTo(std::ostream &os) const {
  const size_t columnCount = columnTypes.size();
  serializeToStream(columnCount, os);

  for (size_t i = 0; i != columnCount; ++i) {
    serializeToStream(*columnTypes.at(i), os);
  }

  const size_t chunkCount = chunks.size();
  serializeToStream(chunkCount, os);

  for (size_t i = 0; i != chunkCount; ++i) {
    serializeHashToStream(chunks[i], os);
  }

  serializeToStream(rowCount, os);
}

PersistentTableMetaDataDeserializer::PersistentTableMetaDataDeserializer(
    std::istream &is)
    : BaseDeserializer(is) {}

PersistentTableMetaData PersistentTableMetaDataDeserializer::getNext() {
  PersistentTableMetaData ptmd;

  const size_t columnCount = deserializeSize(is);
  ptmd.columnTypes.reserve(columnCount);

  TypeDeserializer td(is);
  for (size_t i = 0; i != columnCount; ++i) {
    std::optional<const Type *> columnType = fetchOne(td);
    if (columnType) {
      ptmd.columnTypes.push_back(columnType.value());
    } else {
      throw std::runtime_error(
          "Failed to read column type while parsing persistent table metadata");
    }
  }

  const size_t chunkCount = deserializeSize(is);
  ptmd.chunks.reserve(chunkCount);
  for (size_t i = 0; i != chunkCount; ++i) {
    uint64_t chunkId = deserializeHashFromStream(is);
    ptmd.chunks.push_back(chunkId);
  }

  ptmd.rowCount = deserializeSize(is);
  return ptmd;
}

void initializePersistentTable(IStorage &storage, uint64_t metaDataChunk,
                               std::vector<const Type *> columnTypes) {
  PersistentTableMetaData ptmd;
  ptmd.columnTypes = std::move(columnTypes);
  ptmd.rowCount = 0ULL;

  ptmd.chunks.reserve(initialChunkCount);
  for (size_t i = 0; i < initialChunkCount; ++i) {
    const uint64_t chunkId = allocateChunk(storage);
    clearChunk(storage, chunkId);

    ptmd.chunks.push_back(chunkId);
  }

  storage.writeSerializible(metaDataChunk, ptmd);
}

static void increaseChunkCount(IStorage &storage, uint64_t metaDataChunk,
                               PersistentTableMetaData &ptmd) {
  const size_t chunkCount = 2ULL * ptmd.chunks.size() + 1ULL;
  std::vector<uint64_t> chunks;
  chunks.reserve(chunkCount);

  while (chunks.size() != chunkCount) {
    const uint64_t chunkId = allocateChunk(storage);
    clearChunk(storage, chunkId);

    chunks.push_back(chunkId);
  }

  for (uint64_t chunkId : ptmd.chunks) {
    storage.read(chunkId, [&](std::istream &is) -> void {
      RowDeserializer rowDeserializer(is);
      while (rowDeserializer.hasNext()) {
        const Row row = rowDeserializer.getNext();
        const uint64_t newChunkId =
            chunks[row.getHash() % static_cast<uint64_t>(chunkCount)];
        storage.appendSerializible(newChunkId, row);
      }
    });

    storage.remove(chunkId);
  }

  ptmd.chunks = chunks;
}

void insertRowIntoPersistentTable(IStorage &storage, uint64_t metaDataChunk,
                                  const Row &row) {
  std::optional<PersistentTableMetaData> ptmd;
  storage.read(metaDataChunk, [&](std::istream &is) -> void {
    PersistentTableMetaDataDeserializer ptmdDeserializer(is);
    ptmd = fetchOne(ptmdDeserializer);
  });

  if (!ptmd) {
    throw std::runtime_error("Failed to read persistent table meta data while "
                             "attempting to insert a row");
  }

  uint64_t chunkCount = static_cast<uint64_t>(ptmd->chunks.size());
  uint64_t chunkId = ptmd->chunks[row.getHash() % chunkCount];

  storage.appendSerializible(chunkId, row);

  ptmd->rowCount += 1ULL;
  storage.writeSerializible(metaDataChunk, ptmd.value());

  if (ptmd->rowCount >
      static_cast<uint64_t>(ptmd->chunks.size()) * maxAllowedRowsPerChunk) {
    increaseChunkCount(storage, metaDataChunk, ptmd.value());
  }
}

Table readPersistentTable(IStorage &storage, uint64_t metaDataChunk) {
  std::optional<PersistentTableMetaData> ptmd;
  storage.read(metaDataChunk, [&](std::istream &is) -> void {
    PersistentTableMetaDataDeserializer ptmdDeserializer(is);
    ptmd = fetchOne(ptmdDeserializer);
  });

  if (!ptmd) {
    throw std::runtime_error("Failed to read persistent table meta data while "
                             "attempting to full scan a table into memory");
  }

  Table table(ptmd->columnTypes);
  for (uint64_t chunkId : ptmd->chunks) {
    storage.read(chunkId, [&](std::istream &is) -> void {
      RowDeserializer rowDeserializer(is);
      while (rowDeserializer.hasNext()) {
        table.append(std::move(rowDeserializer.getNext()));
      }
    });
  }

  return table;
}