#include "server/persistent/include/schema.hpp"
#include "server/core/include/meta.hpp"
#include "server/persistent/include/database.hpp"
#include "server/persistent/include/storage.hpp"
#include "server/test/include/case.hpp"
#include "server/test/include/context.hpp"
#include "server/test/include/errors.hpp"

#include <cstdint>
#include <memory>
#include <string>

class SchemeCreateAndFindTestCase : public TestCase, public WithMemoryStorage {
public:
  SchemeCreateAndFindTestCase() = default;

  std::string getName() const override {
    return std::string("persistent/scheme/create-and-find");
  }

  void run() override {
    IStorage &storage = getStorage();

    initializeDataBase(storage);
    const uint64_t schemaMetaDataChunkId =
        createSchemaInDataBase(storage, "schema_0");

    const Type *integerType = getPrimitiveTypeByName("integer");

    const std::vector<Column> columns = {
        Column("col_0", integerType),
        Column("col_1", integerType),
        Column("col_2", integerType),
    };

    createTableInSchema(storage, schemaMetaDataChunkId, "table_0", columns);
    createTableInSchema(storage, schemaMetaDataChunkId, "table_1", columns);
    createTableInSchema(storage, schemaMetaDataChunkId, "table_2", columns);

    assertTrue(static_cast<bool>(
        findTableInSchema(storage, schemaMetaDataChunkId, "table_0")));
    assertTrue(static_cast<bool>(
        findTableInSchema(storage, schemaMetaDataChunkId, "table_1")));
    assertTrue(static_cast<bool>(
        findTableInSchema(storage, schemaMetaDataChunkId, "table_2")));
  }
};

const int _ = []() -> int {
  registerTestCase(std::make_unique<SchemeCreateAndFindTestCase>());
  return 0;
}();
