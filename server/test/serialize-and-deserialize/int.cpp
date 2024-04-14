#include <memory>
#include <sstream>
#include <string>

#include "server/core/include/deserializer.hpp"
#include "server/core/include/serialize.hpp"
#include "server/test/include/case.hpp"
#include "server/test/include/errors.hpp"

class TestSerializeDeserializeInt : public TestCase {
  int i;

public:
  TestSerializeDeserializeInt(int desired) : i(desired) {}

  std::string getName() const override {
    return std::string("serialize-and-deserialize/int/") + std::to_string(i);
  };

  void run() override {
    std::stringstream buf;
    serializeToStream(i, buf);

    const int parsedValue = deserializeInteger(buf);
    assertEqualityByVal(i, parsedValue);
  }
};

const int _ = []() -> int {
  registerTestCase(std::make_unique<TestSerializeDeserializeInt>(42));

  registerTestCase(std::make_unique<TestSerializeDeserializeInt>(100));
  registerTestCase(std::make_unique<TestSerializeDeserializeInt>(1'000));
  registerTestCase(std::make_unique<TestSerializeDeserializeInt>(10'000));
  registerTestCase(std::make_unique<TestSerializeDeserializeInt>(100'000));

  registerTestCase(std::make_unique<TestSerializeDeserializeInt>(1'000'000));
  registerTestCase(std::make_unique<TestSerializeDeserializeInt>(1'030'208));

  registerTestCase(
      std::make_unique<TestSerializeDeserializeInt>(1'030'208'111));
  registerTestCase(
      std::make_unique<TestSerializeDeserializeInt>(2'021'310'321));

  registerTestCase(std::make_unique<TestSerializeDeserializeInt>(0));

  registerTestCase(std::make_unique<TestSerializeDeserializeInt>(-11));
  registerTestCase(std::make_unique<TestSerializeDeserializeInt>(-105));

  return 0;
}();
