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

const uint64_t dataBaseRootChunk = 0U;

struct DataBaseMetaData : public ISerializable {
  std::map<std::string, uint64_t> schemas;

  void writeTo(std::ostream &os) const override;
};

class DataBaseMetaDataDeserializer : public BaseDeserializer<DataBaseMetaData> {
public:
  DataBaseMetaDataDeserializer(std::istream &is);
  ~DataBaseMetaDataDeserializer() = default;

  DataBaseMetaData getNext();
};

void initializeDataBase(IStorage &storage);
uint64_t createSchemaInDataBase(IStorage &storage, std::string name);
std::optional<uint64_t> findSchemaInDataBase(IStorage &storage,
                                             const std::string &name);
