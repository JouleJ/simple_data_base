#include "server/persistent/include/database.hpp"
#include "server/persistent/include/schema.hpp"
#include "server/persistent/include/storage.hpp"
#include "server/persistent/include/table.hpp"
#include "server/query/include/ast.hpp"

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>

Unit CreateTableCommand::execute(IStorage &storage) const {
  std::optional<uint64_t> schemaMetaDataChunkId =
      findSchemaInDataBase(storage, schemaName);
  if (!schemaMetaDataChunkId) {
    throw std::runtime_error(
        std::string("Failed to find schema by such name: ") + schemaName);
  }

  createTableInSchema(storage, schemaMetaDataChunkId.value(), tableName,
                      columns);

  return Unit();
}

Table SelectCommand::execute(IStorage &storage) const {
  std::optional<uint64_t> schemaMetaDataChunkId =
      findSchemaInDataBase(storage, schemaName);
  if (!schemaMetaDataChunkId) {
    throw std::runtime_error(
        std::string("Failed to find schema by such name: ") + schemaName);
  }

  std::optional<uint64_t> tableMetaDataChunkId =
      findTableInSchema(storage, schemaMetaDataChunkId.value(), tableName);
  if (!tableMetaDataChunkId) {
    throw std::runtime_error(std::string("Failed to find table ") + tableName +
                             std::string(" in scheme ") + schemaName);
  }

  if (!columnNames) {
    return readPersistentTable(storage, tableMetaDataChunkId.value());
  }

  const auto filter = [](const Row &row) -> bool {
    return true; /* TODO: Add filtering */
  };

  return selectFromPersistentTable(storage, tableMetaDataChunkId.value(),
                                   columnNames.value(), filter);
}
