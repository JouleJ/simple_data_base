#include "server/persistent/include/table.hpp"
#include "server/core/include/meta.hpp"
#include "server/persistent/include/storage.hpp"
#include "server/persistent/include/utils.hpp"
#include "server/test/include/case.hpp"
#include "server/test/include/context.hpp"
#include "server/test/include/errors.hpp"

#include <iostream>
#include <memory>
#include <string>

class PersistentTableInsertAndFullScanIntoMemoryTestCase
    : public TestCase,
      public WithMemoryStorage {
  std::string name;
  std::vector<Row> rows;

public:
  PersistentTableInsertAndFullScanIntoMemoryTestCase(
      std::string desiredName, std::vector<Row> desiredRows)
      : name(std::move(desiredName)), rows(std::move(desiredRows)) {}

  std::string getName() const override {
    return std::string("persistent/table/insert-and-read/") + name;
  }

  void run() override {
    if (rows.empty()) {
      std::cerr << "[PersistentTableInsertAndFullScanIntoMemoryTestCase::run] "
                   "Cannot run test: no rows\n";
      return;
    }

    IStorage &storage = getStorage();

    const uint64_t metaDataChunk = allocateChunk(storage);
    clearChunk(storage, metaDataChunk);

    initializePersistentTable(storage, metaDataChunk, rows.front().getTypes());
    for (const Row &row : rows) {
      insertRowIntoPersistentTable(storage, metaDataChunk, row);
    }

    const Table memTable = readPersistentTable(storage, metaDataChunk);

    const size_t n = rows.size();
    assertEqualityByVal(memTable.getRowCount(), n);
    for (size_t i = 0; i != n; ++i) {
      assertEqualityByRef<Row>(*memTable.row_at_const(i), rows.at(i));
    }
  }
};

const int _0 = []() -> int {
  std::string name = "small/ints";
  std::vector<Row> rows;
  for (int i = 0; i < 1; ++i) {
    Row row;
    row.append(std::make_unique<IntegerValue>(10 * i));
    row.append(std::make_unique<IntegerValue>(10 * i + 1));
    row.append(std::make_unique<IntegerValue>(10 * i + 2));

    rows.push_back(std::move(row));
  }

  registerTestCase(
      std::make_unique<PersistentTableInsertAndFullScanIntoMemoryTestCase>(
          std::move(name), std::move(rows)));
  return 0;
}();
