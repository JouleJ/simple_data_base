#pragma once

#include "server/core/include/deserializer.hpp"
#include "server/core/include/meta.hpp"
#include "server/core/include/serialize.hpp"
#include "server/persistent/include/storage.hpp"

#include <cstdint>
#include <functional>
#include <istream>
#include <ostream>
#include <vector>

constexpr size_t initialChunkCount = 2ULL;
constexpr size_t maxAllowedRowsPerChunk = 100'000ULL;

struct PersistentTableMetaData : public ISerializable {
  std::vector<Column> columns;
  std::vector<uint64_t> chunks;
  size_t rowCount = 0ULL;

  void writeTo(std::ostream &os) const override;
};

class PersistentTableMetaDataDeserializer
    : public BaseDeserializer<PersistentTableMetaData> {
public:
  PersistentTableMetaDataDeserializer(std::istream &is);
  ~PersistentTableMetaDataDeserializer() override = default;

  PersistentTableMetaData getNext() override;
};

void initializePersistentTable(IStorage &storage, uint64_t metaDataChunk,
                               std::vector<Column> columns);
void insertRowIntoPersistentTable(IStorage &storage, uint64_t metaDataChunk,
                                  const Row &row);
Table readPersistentTable(IStorage &storage, uint64_t metaDataChunk);

void updatePersistentTable(IStorage &storage, uint64_t metaDataChunk,
                           std::function<Row(const Row &row)> mapping);
void eraseFromPersistentTable(IStorage &storage, uint64_t metaDataChunk,
                              std::function<bool(const Row &row)> predicate);
