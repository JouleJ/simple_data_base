#pragma once

#include "server/core/include/deserializer.hpp"
#include "server/core/include/serialize.hpp"
#include "server/persistent/include/storage.hpp"

#include <cstdint>
#include <istream>
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

struct SchemaMetaData : public ISerializable {
  std::map<std::string, uint64_t> tables;

  void writeTo(std::ostream &os) const override;
};

class SchemaMetaDataDeserializer : public BaseDeserializer<SchemaMetaData> {
public:
  SchemaMetaDataDeserializer(std::istream &is);
  ~SchemaMetaDataDeserializer() override = default;

  SchemaMetaData getNext() override;
};

void initializeSchema(IStorage &storage, uint64_t metaDataChunk);
void createTableInSchema(IStorage &storage, uint64_t metaDataChunk,
                         const std::string &tableName,
                         std::vector<Column> columns);

std::optional<uint64_t> findTableInSchema(IStorage &storage,
                                          uint64_t metaDataChunk,
                                          const std::string &tableName);
