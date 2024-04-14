#include "server/core/include/deserializer.hpp"
#include "server/core/include/serialize.hpp"
#include "server/test/include/case.hpp"
#include "server/test/include/errors.hpp"

#include <memory>
#include <sstream>
#include <string>

class TestCaseSerializeDeserializeSize : public TestCase {
  size_t size;

public:
  TestCaseSerializeDeserializeSize(size_t desiredSize) : size(desiredSize) {}

  std::string getName() const override {
    return std::string("serialize-and-deserialize/size/") +
           std::to_string(size);
  }

  void run() override {
    std::stringstream buf;
    serializeToStream(size, buf);

    const size_t parsedValue = deserializeSize(buf);
    assertEqualityByVal(size, parsedValue);
  }
};

const int _ = []() -> int {
  registerTestCase(std::make_unique<TestCaseSerializeDeserializeSize>(42));

  registerTestCase(std::make_unique<TestCaseSerializeDeserializeSize>(0));
  registerTestCase(std::make_unique<TestCaseSerializeDeserializeSize>(1));
  registerTestCase(std::make_unique<TestCaseSerializeDeserializeSize>(2));
  registerTestCase(std::make_unique<TestCaseSerializeDeserializeSize>(3));

  registerTestCase(
      std::make_unique<TestCaseSerializeDeserializeSize>(1'312'980));
  registerTestCase(std::make_unique<TestCaseSerializeDeserializeSize>(
      1'305'102'600ULL * 1'045'504'006UL));

  return 0;
}();
