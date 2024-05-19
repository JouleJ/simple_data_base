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
    assertEqualityOfMultisets(rows, memTable.toVector());
  }
};

const int _0 = []() -> int {
  std::string name = "small/ints";
  std::vector<Row> rows;
  for (int i = 0; i < 10; ++i) {
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

const int _1 = []() -> int {
  std::string name = "big/ints";
  std::vector<Row> rows;
  for (int i = 0; i < 100'000; ++i) {
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

const int _2 = []() -> int {
  std::string name = "big/misc";
  std::vector<Row> rows;
  for (int i = 0; i < 100'000; ++i) {
    Row row;
    row.append(std::make_unique<IntegerValue>(10 * i));
    row.append(std::make_unique<VarcharValue>(std::to_string(15 * i + 6)));
    row.append(std::make_unique<IntegerValue>(10 * i + 2));

    rows.push_back(std::move(row));
  }

  registerTestCase(
      std::make_unique<PersistentTableInsertAndFullScanIntoMemoryTestCase>(
          std::move(name), std::move(rows)));
  return 0;
}();
