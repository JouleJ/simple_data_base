#include "server/core/include/stream.hpp"
#include "server/core/include/utils.hpp"
#include "server/persistent/include/database.hpp"
#include "server/persistent/include/schema.hpp"
#include "server/persistent/include/storage.hpp"
#include "server/persistent/include/table.hpp"
#include "server/query/include/ast.hpp"

#include <cstdint>
#include <istream>
#include <limits>
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

  std::function<bool(const Row &row)> filter;

  if (filterRootNode != nullptr) {
    std::optional<PersistentTableMetaData> ptmd;
    storage.read(*tableMetaDataChunkId, [&](std::istream &is) -> void {
      PersistentTableMetaDataDeserializer ptmdDeserializer(is);
      ptmd = fetchOne(ptmdDeserializer);
    });

    TableContext tableCtx(ptmd->columns);

    filter = [&](const Row &row) -> bool {
      return tableCtx.inferFilter(filterRootNode.get(), &row);
    };
  } else {
    filter = [](const Row &row) -> bool { return true; };
  }

  return selectFromPersistentTable(storage, *tableMetaDataChunkId,
                                   columnNames.value(), filter);
}

Unit InsertCommand::execute(IStorage &storage) const {
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

  if (columnNames.empty()) {
    for (const Row &row : rows) {
      insertRowIntoPersistentTable(storage, *tableMetaDataChunkId, row);
    }

    return Unit();
  }

  std::optional<PersistentTableMetaData> ptmd;
  storage.read(*tableMetaDataChunkId, [&](std::istream &is) {
    PersistentTableMetaDataDeserializer ptmdDeserializer(is);
    ptmd = fetchOne(ptmdDeserializer);
  });

  if (ptmd->columns.size() != columnNames.size()) {
    throw std::runtime_error(
        "Column count in insert query does not match table schema");
  }

  std::map<std::string, size_t> columnNameToIndex;
  VectorStream<Column>(ptmd->columns.begin(), ptmd->columns.end())
      .forEach([&columnNameToIndex](size_t i, const Column &column) {
        columnNameToIndex[column.name] = i;
      });

  std::vector<size_t> indexMapping(ptmd->columns.size(),
                                   std::numeric_limits<size_t>::max());
  for (size_t sourceIndex = 0; sourceIndex != ptmd->columns.size();
       ++sourceIndex) {
    const size_t targetIndex = columnNameToIndex.at(columnNames[sourceIndex]);
    indexMapping[targetIndex] = sourceIndex;
  }

  for (size_t targetIndex = 0; targetIndex != ptmd->columns.size();
       ++targetIndex) {
    if (indexMapping[targetIndex] == std::numeric_limits<size_t>::max()) {
      throw std::runtime_error(
          std::string("Following column missing in insert query: ") +
          quote(ptmd->columns[targetIndex].name));
    }
  }

  for (const Row &row : rows) {
    Row rearrangedRow;
    for (size_t targetIndex = 0; targetIndex != ptmd->columns.size();
         ++targetIndex) {
      const size_t sourceIndex = indexMapping[targetIndex];
      rearrangedRow.append(row.at_const(sourceIndex)->copy());
    }

    insertRowIntoPersistentTable(storage, *tableMetaDataChunkId, rearrangedRow);
  }

  return Unit();
}
