#include <algorithm>
#include <memory>
#include <sstream>
#include <string>

#include "server/core/include/deserializer.hpp"
#include "server/core/include/meta.hpp"
#include "server/core/include/serialize.hpp"
#include "server/test/include/case.hpp"
#include "server/test/include/errors.hpp"

class TestCaseSerializeDeserializeRow : public TestCase {
  Row row;

public:
  TestCaseSerializeDeserializeRow(Row desiredRow)
      : row(std::move(desiredRow)) {}

  std::string getName() const override {
    return std::string("serialize-and-deserialize/row/") + row.toString();
  }

  void run() override {
    std::stringstream buf;
    serializeToStream(row, buf);

    const Row parsedRow = RowDeserializer(buf).getNext();
    assertEqualityByRef(row, parsedRow);
  }
};

const int _ = []() -> int {
  {
    Row emptyRow;
    registerTestCase(
        std::make_unique<TestCaseSerializeDeserializeRow>(std::move(emptyRow)));
  }

  {
    Row singularIntegerRow;
    singularIntegerRow.append(std::make_unique<IntegerValue>(42));
    registerTestCase(std::make_unique<TestCaseSerializeDeserializeRow>(
        std::move(singularIntegerRow)));
  }

  {
    Row singularVarcharRow;
    singularVarcharRow.append(std::make_unique<VarcharValue>("hello world"));
    registerTestCase(std::make_unique<TestCaseSerializeDeserializeRow>(
        std::move(singularVarcharRow)));
  }

  {
    Row row;
    row.append(std::make_unique<IntegerValue>(43));
    row.append(std::make_unique<VarcharValue>("hello"));
    row.append(std::make_unique<IntegerValue>(-1));
    row.append(std::make_unique<IntegerValue>(15));
    row.append(std::make_unique<VarcharValue>("\nworld"));

    registerTestCase(
        std::make_unique<TestCaseSerializeDeserializeRow>(std::move(row)));
  }

  return 0;
}();
